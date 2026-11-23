using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using MocapApi;
using UnityEngine;
using UnityEngine.UI;

// Private probe only: native networking is disabled before Start. Never included in delivery.
public sealed class SoloWorkflowRuntimeProbe : MonoBehaviour
{
    string output;
    int checks;
    bool done;
    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.AfterSceneLoad)]
    static void Install()
    {
        foreach(var app in UnityEngine.Object.FindObjectsByType<MocapApplication>(FindObjectsInactive.Include,FindObjectsSortMode.None)) app.enabled=false;
        new GameObject("SoloWorkflowRuntimeProbe").AddComponent<SoloWorkflowRuntimeProbe>();
    }
    void Awake()
    {
        Application.runInBackground=true;
        var args=Environment.GetCommandLineArgs(); output=args[Array.IndexOf(args,"--guide-probe-output")+1];
        Directory.CreateDirectory(output); Application.logMessageReceived+=OnLog;
    }
    void OnLog(string message,string trace,LogType type)
    {
        if(!done && (type==LogType.Error || type==LogType.Exception || type==LogType.Assert)) {
            done=true; File.WriteAllText(Path.Combine(output,"failure.txt"),message+"\n"+trace); Application.Quit(7);
        }
    }
    void Check(bool value,string message) { if(!value)throw new Exception("PROBE: "+message); ++checks; }
    IEnumerator Shot(string name)
    {
        yield return null; yield return new WaitForEndOfFrame();
        var texture=ScreenCapture.CaptureScreenshotAsTexture();
        File.WriteAllBytes(Path.Combine(output,name+".png"),texture.EncodeToPNG()); Destroy(texture);
    }
    object Field(object obj,string name) { return obj.GetType().GetField(name,BindingFlags.NonPublic|BindingFlags.Instance).GetValue(obj); }
    IEnumerator Start()
    {
        yield return null; yield return null; yield return null;
        var app=UnityEngine.Object.FindFirstObjectByType<MocapApplication>(FindObjectsInactive.Include);
        var main=app.GetComponent<MainPanel>(); var workspace=main.Workspace; var session=app.GetComponent<MotionSession>();
        Check(!app.enabled,"native connection disabled");
        Check(workspace!=null && workspace.SelectedTab==0,"live workspace selected");
        Check(!main.ControlToolbar.activeSelf && !main.ControlPanel.activeSelf,"legacy entry points replaced");
        yield return Shot("01-live");
        int invoked=0;
        main.PrepareAction("恢复姿态测试","请摆好姿态。",()=>++invoked);
        Check(main.ActionPrompt.IsPending,"preparation starts immediately");
        main.PrepareAction("重复请求","",()=>invoked+=100);
        yield return Shot("02-five-second-preparation");
        yield return new WaitForSecondsRealtime(4.6f);
        Check(invoked==0,"no effect before five seconds");
        yield return new WaitForSecondsRealtime(.6f);
        Check(invoked==1 && !main.ActionPrompt.IsPending,"only first action executes once after five seconds");
        main.PrepareAction("取消测试","",()=>++invoked); main.CancelPreparation();
        yield return new WaitForSecondsRealtime(.1f);
        Check(!main.ActionPrompt.IsPending && invoked==1,"cancel discards pending operation");
        main.handleCalibration();
        Check(Field(main,"calibrationProcess")==null,"no calibration command allocated during local countdown");
        main.handleCalibration();
        yield return new WaitForSecondsRealtime(5.3f);
        var process=(CalibrationProcess)Field(main,"calibrationProcess");
        Check(process!=null && process.isComplete() && process.Guide.Phase==CalibrationGuidePhase.Failed,"disconnected delayed calibration fails visibly and releases command");
        Check(((Dictionary<ulong,AbstractCommandProcess>)Field(app,"pendingCommandMap")).Count==0,"failed request leaves no pending native command");
        ((CalibrationGuidePanel)Field(main,"calibrationGuide")).Hide();
        var guide=new CalibrationGuideState(); guide.SetSupportedPoses(new[]{"A-Pose","T-Pose","F-Pose"});
        var panel=CalibrationGuidePanel.Create(main.transform); panel.Show(guide);
        guide.Report(CalibrationGuidePhase.Capturing,"F-Pose",65);
        yield return Shot("03-fpose-walking");
        Check(guide.IsWalkingPose && guide.Message.Contains("走路") && !guide.Message.Contains("不动"),"F pose instructs walking");
        guide.Report(CalibrationGuidePhase.Capturing,"F-Pose",100);
        yield return Shot("04-processing-free-to-move");
        Check(guide.Phase==CalibrationGuidePhase.Processing && guide.IsRunning,"collection completion is processing, not success");
        Check(panel.transform.Find("ModalShade/GuideCard/Pose").GetComponent<Text>().text=="可以自由活动","large feedback permits free movement");
        guide.Report(CalibrationGuidePhase.Capturing,"F-Pose",60);
        Check(guide.Phase==CalibrationGuidePhase.Processing,"late progress does not undo processing");
        guide.Finish(true,"完成"); yield return null;
        Check(guide.Phase==CalibrationGuidePhase.Succeeded,"only command result marks success"); panel.Hide();
        workspace.ShowTab(1); yield return Shot("05-recordings-empty");
        var model=app.avatarGameObject.transform;
        Vector3 original=model.localPosition; Quaternion originalRotation=model.localRotation;
        session.NotifyLiveFrame(); session.StartRecording(); Check(session.IsRecording,"recording starts when data is recent");
        model.localPosition=(original+new Vector3(.1f,0,.2f)); model.localRotation=originalRotation*Quaternion.Euler(0,20,0);
        session.NotifyLiveFrame(); yield return new WaitForSecondsRealtime(.2f);
        model.localPosition=(original+new Vector3(.3f,0,.4f)); model.localRotation=originalRotation*Quaternion.Euler(0,50,0); session.NotifyLiveFrame();
        yield return Shot("06-recording");
        session.StopRecording(); string take=session.CurrentFile;
        Check(!session.IsRecording && File.Exists(take),"ending recording finalizes local file");
        string saved=Path.Combine(output,"synthetic-motion.mocap"); File.Move(take,saved);
        using(var reader=new MotionRecordingReader(saved)) {
            Check(reader.Frames==2 && reader.Duration>.15,"event timestamps and all frames retained");
            Check(reader.Paths.Length>20,"actual scene bone hierarchy stored");
        }
        session.LoadRecording(saved); yield return new WaitUntil(()=>!session.IsLoading); yield return null;
        Check(session.IsPlayback && !session.Playing,"load starts paused");
        Check(Vector3.Distance(model.localPosition,(original+new Vector3(.1f,0,.2f)))<.00001f,"first pose exactly restored");
        session.Seek(session.Duration); yield return null;
        Check(Vector3.Distance(model.localPosition,(original+new Vector3(.3f,0,.4f)))<.00001f,"seek restores final pose");
        var ev=new MCPEvent_t[1]; ev[0].eventType=EMCPEventType.MCPEvent_AvatarUpdated;
        app.DispatchEvents(ev,1); // Invalid handle would fail if live pose code ran during playback.
        Check(session.IsPlayback,"live motion events isolated from playback");
        int actionCount=0; main.PrepareAction("设备命令","",()=>++actionCount);
        Check(!main.ActionPrompt.IsPending && actionCount==0,"device action blocked in playback");
        session.TogglePlay(); yield return new WaitForSecondsRealtime(.08f);
        Check(session.Position>0,"play advances timeline");
        session.Speed=.5f; session.Seek(0); session.TogglePlay(); // currently playing -> pause
        Check(!session.Playing,"pause works");
        yield return Shot("07-playback-paused");
        session.ReturnLive(); Check(!session.IsPlayback,"return live exits playback");
        Check(Vector3.Distance(model.localPosition,(original+new Vector3(.3f,0,.4f)))<.00001f,"return live restores last live pose");
        session.LoadRecording(saved); session.ReturnLive(); yield return new WaitUntil(()=>!session.IsLoading);
        Check(!session.IsPlayback,"return live cancels background loading without late reentry");
        session.LoadRecording(Path.Combine(output,"missing.mocap")); yield return new WaitUntil(()=>!session.IsLoading);
        Check(!session.IsPlayback,"missing recording fails visibly without leaving live mode");
        session.NotifyLiveFrame(); session.StartRecording(); session.NotifyLiveFrame();
        string autoSaved=session.CurrentFile; session.SendMessage("OnApplicationQuit");
        Check(!session.IsRecording && File.Exists(autoSaved),"normal application exit finalizes recording automatically");
        File.Move(autoSaved,Path.Combine(output,"synthetic-auto-save.mocap"));
        model.localPosition=original; model.localRotation=originalRotation;
        workspace.ShowTab(2); yield return Shot("08-connection"); workspace.ShowTab(3); yield return Shot("09-tools");
        Check(Camera.main.rect.x>.27f && Camera.main.rect.height<.8f,"model has unobscured viewport");
        done=true; File.WriteAllText(Path.Combine(output,"passed.json"),"{\"passed\":true,\"checks\":"+checks+",\"device_commands_sent\":0,\"screenshots\":9}");
        Application.Quit(0);
    }
}
