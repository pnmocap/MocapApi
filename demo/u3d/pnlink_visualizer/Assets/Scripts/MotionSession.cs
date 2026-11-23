using System;
using System.IO;
using System.Collections.Generic;
using System.Diagnostics;
using UnityEngine;
using System.Threading;
using System.Threading.Tasks;

public sealed class MotionSession : MonoBehaviour
{
    MainPanel main;
    Transform[] bones, playbackBones;
    string[] paths;
    float[] values, playbackValues;
    public GameObject PlaybackModel { get; private set; }
    public bool PlaybackVisible { get; private set; }
    public DeviceSnapshot PlaybackStatus { get; private set; }
    public int RecordingVersion { get { return reader==null?0:reader.Version; } }
    double nextStatus=-1;
    public void InitializeViews()
    {
        if(PlaybackModel!=null)return;
        var original=GetComponent<MocapApplication>().avatarGameObject;
        PlaybackModel=Instantiate(original,original.transform.parent);
        PlaybackModel.name="PlaybackAvatar"; PlaybackModel.SetActive(false);
        var controller=PlaybackModel.GetComponent<AvatarController>(); if(controller!=null)Destroy(controller);
        BindBones();
    }
    public void SetPage(int tab)
    {
        InitializeViews(); PlaybackVisible=tab==1;
        if(!PlaybackVisible)Playing=false;
        GetComponent<MocapApplication>().avatarGameObject.SetActive(tab==0);
        PlaybackModel.SetActive(tab==1 && IsPlayback);
    }
    MotionRecordingWriter writer;
    MotionRecordingReader reader;
    Task<MotionRecordingReader> loading;
    CancellationTokenSource loadCancellation;
    string loadingPath;
    public bool IsLoading { get { return loading != null; } }
    double recordingStart = -1, lastTime = -1;
    long lastFrame;
    bool pendingFrame;
    public bool IsRecording { get { return writer != null; } }
    public bool IsPlayback { get { return reader != null; } }
    public bool Playing { get; private set; }
    public double Position { get; private set; }
    public double Duration { get { return reader == null ? 0 : reader.Duration; } }
    public double RecordingSeconds { get { return recordingStart < 0 ? 0 : Math.Max(0, Clock-recordingStart); } }
    public float Speed = 1;
    public string CurrentFile { get; private set; } = "";
    public string DirectoryPath { get { return Path.Combine(Application.persistentDataPath,"Recordings"); } }
    public bool HasRecentData { get { return lastFrame != 0 && Stopwatch.GetTimestamp()-lastFrame < 2*Stopwatch.Frequency; } }
    static double Clock { get { return Stopwatch.GetTimestamp()/(double)Stopwatch.Frequency; } }
    void Awake() { main = GetComponent<MainPanel>(); }
    void BindBones()
    {
        if (bones != null) return;
        var model = GetComponent<MocapApplication>().avatarGameObject.transform;
        var list = new List<Transform> { model }; var names = new List<string> { "" };
        foreach (Transform t in model.GetComponentsInChildren<Transform>(true)) {
            if (t == model || !t.name.StartsWith("Noitom_",StringComparison.Ordinal)) continue;
            var parts = new List<string>(); var cursor = t;
            while (cursor != model) { parts.Add(cursor.name); cursor=cursor.parent; }
            parts.Reverse(); names.Add(string.Join("/",parts.ToArray())); list.Add(t);
        }
        bones = list.ToArray(); paths=names.ToArray(); values=new float[bones.Length*7];
    }
    static void Capture(Transform[] source, float[] target)
    {
        for (int i=0;i<source.Length;i++) {
            Vector3 p=source[i].localPosition; Quaternion q=source[i].localRotation; int k=i*7;
            target[k]=p.x; target[k+1]=p.y; target[k+2]=p.z;
            target[k+3]=q.x; target[k+4]=q.y; target[k+5]=q.z; target[k+6]=q.w;
        }
    }
    static void Apply(Transform[] target, float[] source)
    {
        for (int i=0;i<target.Length;i++) {
            int k=i*7; target[i].localPosition=new Vector3(source[k],source[k+1],source[k+2]);
            target[i].localRotation=new Quaternion(source[k+3],source[k+4],source[k+5],source[k+6]);
        }
    }
    public void NotifyLiveFrame()
    {
        lastFrame=Stopwatch.GetTimestamp();
        if (writer == null) return;
        try {
            double now = Clock;
            if (recordingStart < 0) recordingStart=now;
            double time=Math.Max(now-recordingStart,lastTime+0.000001);
            byte[] status=null;
            if(now>=nextStatus) { status=GetComponent<DeviceMonitor>().Capture(HasRecentData).Encode(); nextStatus=now+1; }
            Capture(bones,values); writer.Write(time,values,status); lastTime=time;
            if (writer.Bytes > 2L*1024*1024*1024 || writer.Frames >= 990000) {
                StopRecording(); main.ReportWarning("单段录制达到容量上限，已保存。可开始下一段录制。");
            }
        } catch (Exception e) { StopRecording(); main.ReportError("录制已停止："+e.Message); }
    }
    public void StartRecording()
    {
        if (IsRecording || PlaybackVisible || IsLoading) return;
        if (!HasRecentData) { main.ReportWarning("尚未收到实时动作，请先开始采集。"); return; }
        try {
            BindBones(); Directory.CreateDirectory(DirectoryPath);
            CurrentFile=Path.Combine(DirectoryPath,"Take-"+DateTime.Now.ToString("yyyyMMdd-HHmmss-fff")+".mocap");
            writer=new MotionRecordingWriter(CurrentFile+".partial",paths,true); recordingStart=-1; lastTime=-1; nextStatus=-1;
            main.ReportInfo("本机录制已开始。");
        } catch(Exception e) { main.ReportError("无法开始录制："+e.Message); }
    }
    public void StopRecording()
    {
        if (writer == null) return;
        var active=writer; writer=null;
        try {
            int count=active.Frames; active.Dispose();
            if (count > 0) { File.Move(active.Path,CurrentFile); main.ReportInfo("已保存："+CurrentFile); }
            else { File.Delete(active.Path); main.ReportWarning("没有收到动作帧，未生成空录像。"); }
        } catch(Exception e) { main.ReportError("录像结束异常，已写入数据保留为 .partial："+e.Message); }
    }
    public void LoadRecording(string path)
    {
        if(IsLoading) { main.ReportWarning("正在读取录像，请稍候。"); return; }
        if (IsRecording) { main.ReportWarning("请先结束录制再回放。"); return; }
        if (main.IsCalibrating || (main.ActionPrompt != null && main.ActionPrompt.IsPending) || GetComponent<MocapApplication>().HasPendingCommands) {
            main.ReportWarning("请先结束当前设备操作，再进入回放。"); return;
        }
        try {
            InitializeViews(); Playing=false; loadingPath=path; loadCancellation=new CancellationTokenSource();
            var token=loadCancellation.Token;
            // Only file validation runs off the main thread; Unity transforms stay on the main thread.
            loading=Task.Run(()=>new MotionRecordingReader(path,token));
            main.ReportInfo("正在读取并校验录像，可随时点击返回实时取消。");
        } catch(Exception e) { main.ReportError("无法打开录像："+e.Message); }
    }
    void CompleteLoad()
    {
        MotionRecordingReader candidate=null;
        try {
            candidate=loading.GetAwaiter().GetResult();
            if(loadCancellation.IsCancellationRequested) return;
            var map=new Dictionary<string,Transform>();
            for (int i=0;i<paths.Length;i++) map.Add(paths[i],paths[i]==""?PlaybackModel.transform:PlaybackModel.transform.Find(paths[i]));
            if (candidate.Paths.Length != paths.Length) throw new InvalidDataException("录像骨骼与当前模型不一致。");
            var bound=new Transform[candidate.Paths.Length];
            for (int i=0;i<bound.Length;i++) if (!map.TryGetValue(candidate.Paths[i],out bound[i]) || bound[i]==null) throw new InvalidDataException("录像骨骼与当前模型不一致。");
            if (IsPlayback) reader.Dispose();
            reader=candidate; candidate=null; playbackBones=bound; playbackValues=new float[values.Length];
            PlaybackModel.SetActive(PlaybackVisible);
            CurrentFile=loadingPath; Position=0; Playing=false; pendingFrame=true;
            main.ReportInfo(reader.Recovered ? "已恢复完整录像帧。尾部未完成的数据不会播放。" : "录像已载入，点击播放。");
        } catch(OperationCanceledException) { main.ReportInfo("已取消载入录像。"); }
        catch(Exception e) { main.ReportError("无法打开录像："+e.Message); }
        finally { if(candidate != null) candidate.Dispose(); loading=null; loadCancellation.Dispose(); loadCancellation=null; }
    }
    public void TogglePlay() { if (reader == null || IsLoading || !PlaybackVisible) return; if (Position >= Duration) Seek(0); Playing=!Playing; }
    public void Seek(double seconds) { if(reader==null)return; Position=Math.Max(0,Math.Min(Duration,seconds)); pendingFrame=true; }
    public void ReturnLive()
    {
        if(IsLoading)loadCancellation.Cancel();
        if (reader==null) return;
        reader.Dispose(); reader=null; Playing=false;
        PlaybackModel.SetActive(false);PlaybackStatus=null; main.ReportInfo("已返回实时，等待新的动作数据。");
    }
    void Update()
    {
        if(loading!=null && loading.IsCompleted) CompleteLoad();
        if (reader==null || !PlaybackVisible) return;
        if (Playing) {
            Position=Math.Min(Duration,Position+Time.unscaledDeltaTime*Speed); pendingFrame=true;
            if(Position>=Duration)Playing=false;
        }
        if (!pendingFrame) return;
        try { int frame=reader.FrameAt(Position); reader.Read(frame,playbackValues); Apply(playbackBones,playbackValues); PlaybackStatus=reader.SnapshotAt(frame); pendingFrame=false; }
        catch(Exception e) { ReturnLive(); main.ReportError("回放停止："+e.Message); }
    }
    void OnApplicationQuit() { StopRecording(); }
    void OnDestroy() {
        StopRecording(); if(reader!=null)reader.Dispose(); if(PlaybackModel!=null)Destroy(PlaybackModel);
        if(loading!=null) {
            var cancel=loadCancellation; cancel.Cancel();
            loading.ContinueWith(task=>{ if(task.Status==TaskStatus.RanToCompletion)task.Result.Dispose(); else { var observed=task.Exception; } cancel.Dispose(); },TaskScheduler.Default);
        }
    }
}
