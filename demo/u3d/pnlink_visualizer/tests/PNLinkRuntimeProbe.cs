using System;
using System.Collections;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using UnityEngine;
using UnityEngine.UI;

// Private test player only. The native device connection is disabled before Start.
public sealed class PNLinkRuntimeProbe : MonoBehaviour
{
    string output;int checks;bool done;
    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.AfterSceneLoad)]
    static void Install()
    {
        foreach(var app in UnityEngine.Object.FindObjectsByType<MocapApplication>(FindObjectsInactive.Include,FindObjectsSortMode.None))app.enabled=false;
        new GameObject("PNLinkRuntimeProbe").AddComponent<PNLinkRuntimeProbe>();
    }
    void Awake()
    {
        Application.runInBackground=true;var args=Environment.GetCommandLineArgs();output=args[Array.IndexOf(args,"--guide-probe-output")+1];Directory.CreateDirectory(output);
        Application.logMessageReceived+=(msg,trace,type)=>{
            if(!done&&(type==LogType.Exception||type==LogType.Error||type==LogType.Assert)){done=true;File.WriteAllText(Path.Combine(output,"failure.txt"),msg+"\n"+trace);Application.Quit(7);}
        };
    }
    void Check(bool yes,string message){if(!yes)throw new Exception("PROBE: "+message);++checks;}
    object Field(object obj,string name){return obj.GetType().GetField(name,BindingFlags.NonPublic|BindingFlags.Instance).GetValue(obj);}
    void Snapshot(DeviceMonitor monitor,string serial,int mag)
    {
        var s=new DeviceSnapshot{Available=true,Online=true,Serial=serial,Firmware="1.2.3",Sensors=new[]{new SensorSnapshot{Id=7,Bone="RightForeArm",Serial="SENSOR-7",Firmware="1.0.0",PacketRate=99.5f,Magnetic=mag}}};
        typeof(DeviceMonitor).GetField("current",BindingFlags.NonPublic|BindingFlags.Instance).SetValue(monitor,s);
        typeof(DeviceMonitor).GetField("updated",BindingFlags.NonPublic|BindingFlags.Instance).SetValue(monitor,Time.realtimeSinceStartupAsDouble);
    }
    IEnumerator Shot(string name)
    {
        yield return new WaitForSecondsRealtime(.30f);yield return new WaitForEndOfFrame();
        var texture=ScreenCapture.CaptureScreenshotAsTexture();File.WriteAllBytes(Path.Combine(output,name+".png"),texture.EncodeToPNG());Destroy(texture);
    }
    IEnumerator Start()
    {
        yield return null;yield return null;yield return null;
        var app=UnityEngine.Object.FindFirstObjectByType<MocapApplication>(FindObjectsInactive.Include);var main=app.GetComponent<MainPanel>();
        var w=main.Workspace;var session=app.GetComponent<MotionSession>();var monitor=app.GetComponent<DeviceMonitor>();
        if(VLoc.English)VLoc.Toggle();
        Check(!app.enabled&&w.IsReady,"no native networking; new workspace initialized");
        Check(Application.version==VisualizerBuildInfo.Version,"Unity Application.version equals the product version");
        Check(Regex.IsMatch(VisualizerBuildInfo.Version,@"^\d+\.\d+\.\d+$"),"product version uses semantic versioning");
        Check(MocapApi.Version.libMocapApiString==MocapApi.Version.mocapApiString,"native MocapApi and C# binding versions match");
        Check(!main.SettingsPanel.activeSelf&&!main.DevicePanel.activeSelf,"legacy settings and device windows replaced");
        Check(w.LiveCamera!=w.ReplayCamera&&session.PlaybackModel!=app.avatarGameObject,"separate camera and avatar instances");
        Snapshot(monitor,"LIVE-A",1);session.NotifyLiveFrame();yield return Shot("01-live-zh");
        Check(((Image)Field(w,"connectionDot")).color.g>.7f,"home green dot follows actual received motion");
        var root=(Canvas)Field(w,"canvas");
        Check(!root.transform.Find("Footer1").gameObject.activeInHierarchy,"live page has no playback bar");
        Check(root.GetComponentsInChildren<Button>(true).Count(b=>b.name=="Capture")==1,"single start capture entry");
        Check(!root.GetComponentsInChildren<Button>(true).Any(b=>b.name=="Ptp"||b.name=="MotionOptions"||b.name=="Devices"),"duplicate capture, ambiguous motion tool and connection sensor entry removed");
        int called=0;main.PrepareAction("恢复身体姿态","请摆好要恢复的身体姿态。",()=>called++);
        VLoc.Toggle();yield return Shot("02-countdown-en");
        Check(main.ActionPrompt.GetComponentInChildren<Text>().text.Contains("Restore body"),"English countdown operation translated");
        main.PrepareAction("恢复手部姿态","",()=>called+=100);
        yield return new WaitForSecondsRealtime(4.3f);Check(called==0,"no action before five seconds");
        yield return new WaitForSecondsRealtime(.7f);Check(called==1,"duplicate click executes original action only once");
        Check(((Image)Field(w,"connectionDot")).color.g<.7f,"connection dot expires without received motion");
        main.PrepareAction("恢复手部姿态","请摆好要恢复的手部姿态。",()=>called++);main.CancelPreparation();Check(!main.ActionPrompt.IsPending,"countdown cancels");
        var guide=new CalibrationGuideState();guide.SetSupportedPoses(new[]{"A-Pose","T-Pose","F-Pose"});
        var panel=CalibrationGuidePanel.Create(main.transform);panel.Show(guide);guide.Report(CalibrationGuidePhase.Capturing,"FPose",65);
        yield return Shot("03-walk-en");Check(guide.Message.Contains("自然向前走路"),"walking segment instructs walking");
        guide.Report(CalibrationGuidePhase.Capturing,"FPose",80);yield return Shot("04-stand-still-en");
        Check(guide.IsStandingFinish&&panel.transform.Find("ModalShade/GuideCard/Phase").GetComponent<Text>().text.Contains("stand still"),"final segment clearly requires standing still in English");
        guide.Report(CalibrationGuidePhase.Capturing,"FPose",99);Check(guide.IsStandingFinish&&guide.IsRunning,"standing continues through 99 percent");
        guide.Report(CalibrationGuidePhase.Capturing,"FPose",100);yield return Shot("05-free-to-move-en");
        Check(guide.Phase==CalibrationGuidePhase.Processing&&guide.IsRunning,"free movement only after collection completion; not final success");panel.Hide();
        VLoc.Toggle();
        var model=app.avatarGameObject.transform;Vector3 original=model.localPosition;Quaternion rotation=model.localRotation;
        Snapshot(monitor,"REC-A",1);session.NotifyLiveFrame();session.StartRecording();
        Check(session.IsRecording,"recording controls belong to live page");
        model.localPosition=original+new Vector3(.1f,0,.2f);session.NotifyLiveFrame();yield return new WaitForSecondsRealtime(.2f);
        model.localPosition=original+new Vector3(.2f,0,.3f);session.NotifyLiveFrame();yield return new WaitForSecondsRealtime(1.05f);
        Snapshot(monitor,"REC-B",3);model.localPosition=original+new Vector3(.3f,0,.4f);session.NotifyLiveFrame();session.StopRecording();
        string recorded=Path.Combine(output,"synthetic-r8.mocap");File.Move(session.CurrentFile,recorded);
        using(var r=new MotionRecordingReader(recorded)){
            Check(r.Version==2&&r.Frames==3,"new format records frames and metadata");
            Check(r.SnapshotAt(0).Serial=="REC-A"&&r.SnapshotAt(2).Serial=="REC-B","recording retains changing device snapshots");
            var pose=new float[r.Paths.Length*7];r.Read(0,pose);
            using(var legacy=new MotionRecordingWriter(Path.Combine(output,"synthetic-r7.mocap"),r.Paths))legacy.Write(0,pose);
        }
        Snapshot(monitor,"LIVE-NOW",2);w.ShowTab(1);session.LoadRecording(recorded);yield return new WaitUntil(()=>!session.IsLoading);yield return null;
        Check(session.IsPlayback&&!app.avatarGameObject.activeSelf&&session.PlaybackModel.activeSelf,"replay has its own visible model; live model hidden");
        Check(session.PlaybackStatus.Serial=="REC-A","replay status comes from the take, never the current device");
        Check(Vector3.Distance(session.PlaybackModel.transform.localPosition,original+new Vector3(.1f,0,.2f))<.00001f,"replay restores recorded first pose");
        model.localPosition=original+Vector3.one;session.NotifyLiveFrame();
        Check(Vector3.Distance(session.PlaybackModel.transform.localPosition,model.localPosition)>.1f,"live transforms do not overwrite replay");
        session.Seek(session.Duration);yield return null;
        Check(session.PlaybackStatus.Serial=="REC-B"&&session.PlaybackStatus.Sensors[0].Magnetic==3,"seeking switches historical sensor state");
        yield return Shot("06-replay-zh");
        var replayStatus=(DeviceStatusView)Field(w,"replayStatus");
        var row=replayStatus.transform.Find("Sensors/Viewport/Content/Sensor0").GetComponent<Button>();
        row.onClick.Invoke();yield return Shot("06b-sensor-detail-zh");
        var detailCanvas=(Canvas)Field(replayStatus,"details");
        Check(detailCanvas.GetComponentsInChildren<Text>().Any(t=>t.text.Contains("SENSOR-7")),"readable sensor detail uses recorded serial and firmware");
        Check(((RectTransform)detailCanvas.transform.Find("Shade/Card")).rect.width>600,"sensor detail is a large independent modal, not clipped by the status column");
        replayStatus.HideDetail();
        Check(!root.transform.Find("Footer0").gameObject.activeInHierarchy&&root.transform.Find("Footer1").gameObject.activeInHierarchy,"replay transport exists only on replay page");
        Vector3 replayPosition=w.ReplayCamera.transform.position;w.ShowTab(0);w.LiveCamera.transform.position+=new Vector3(.01f,0,0);
        Check(w.ReplayCamera.transform.position==replayPosition,"camera states are independent");
        Check(app.avatarGameObject.activeSelf&&!session.PlaybackModel.activeSelf,"live tab restores only live view");
        main.PrepareAction("恢复身体姿态","请摆好要恢复的身体姿态。",()=>called++);Check(main.ActionPrompt.IsPending,"loaded replay does not block live page actions");main.CancelPreparation();
        w.ShowTab(1);Check(session.PlaybackStatus.Serial=="REC-B","tab switch retains replay timeline and historical state");
        session.LoadRecording(Path.Combine(output,"synthetic-r7.mocap"));yield return new WaitUntil(()=>!session.IsLoading);yield return null;
        Check(session.RecordingVersion==1&&session.PlaybackStatus==null,"legacy r7 recordings load without invented sensor data");
        session.LoadRecording(recorded);session.ReturnLive();yield return new WaitUntil(()=>!session.IsLoading);Check(!session.IsPlayback,"cancel loading prevents late re-entry");
        w.ShowTab(2);yield return Shot("07-connection-zh");
        Check(!w.LiveCamera.enabled&&!w.ReplayCamera.enabled,"connection page has no 3D camera");
        Check(!root.transform.Find("Footer1").gameObject.activeInHierarchy,"connection page has no timeline");
        var ip=(InputField)Field(w,"localIP");Check(ip.textComponent.fontSize>=20,"large readable IP input");
        string oldIP=ip.text;ip.text="invalid";Check(!w.ApplySettings(),"invalid endpoints rejected before reconnect");ip.text=oldIP;
        VLoc.Toggle();yield return Shot("08-connection-en");
        Check(root.transform.Find("Center2/Title").GetComponent<Text>().text=="Network & capture settings","connection form switches to English");
        w.ShowTab(3);yield return Shot("09-tools-en");
        Check(!w.LiveCamera.enabled&&!w.ReplayCamera.enabled&&!root.transform.Find("Footer1").gameObject.activeInHierarchy,"tools have no 3D or playback bar");
        w.ShowTab(0);yield return Shot("10-live-en");
        foreach(var text in root.GetComponentsInChildren<Text>())if(text.gameObject.activeInHierarchy&&!text.name.Equals("Caption")) {
            if(Regex.IsMatch(text.text,@"[\u4e00-\u9fff]"))throw new Exception("Untranslated visible text: "+text.name+"="+text.text);
        }
        Check(true,"visible English live interface contains no Chinese labels");
        model.localPosition=original;model.localRotation=rotation;
        done=true;File.WriteAllText(Path.Combine(output,"passed.json"),"{\"passed\":true,\"checks\":"+checks+",\"device_commands_sent\":0,\"screenshots\":11}");Application.Quit(0);
    }
}
