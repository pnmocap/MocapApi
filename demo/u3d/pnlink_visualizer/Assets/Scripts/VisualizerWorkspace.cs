using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Rendering.Universal;

public sealed class VisualizerWorkspace : MonoBehaviour
{
    MainPanel main; MotionSession session; DeviceMonitor monitor;
    Canvas canvas;
    readonly GameObject[] pages=new GameObject[4], centers=new GameObject[4], footers=new GameObject[4];
    readonly List<Button> deviceActions=new List<Button>();
    Camera liveCamera,replayCamera; CameraController liveOrbit,replayOrbit;
    DeviceStatusView liveStatus,replayStatus;
    Text status,mode,clock,filePage,fileName,connectionState,logs,info;
    Image connectionDot;
    Transform library;
    Slider timeline;
    Button record,stopRecord,play,apply;
    Text playCaption,speedCaption,ptpCaption;
    InputField externalPath,localIP,localPort,remoteIP,remotePort;
    bool ptp, titleSet;
    int fileIndex;
    float nextRefresh;
    string message=""; bool error;
    readonly Queue<string> messages=new Queue<string>();
    public int SelectedTab {get;private set;}
    public bool IsReady {get;private set;}
    public Camera LiveCamera {get{return liveCamera;}}
    public Camera ReplayCamera {get{return replayCamera;}}
    [DllImport("user32.dll",CharSet=CharSet.Unicode)]static extern bool SetWindowText(IntPtr window,string title);
    delegate bool WindowCallback(IntPtr window,IntPtr data);
    [DllImport("user32.dll")]static extern bool EnumWindows(WindowCallback callback,IntPtr data);
    [DllImport("user32.dll")]static extern bool IsWindowVisible(IntPtr window);
    [DllImport("user32.dll")]static extern uint GetWindowThreadProcessId(IntPtr window,out uint pid);
    void Start()
    {
        VLoc.Load();main=GetComponent<MainPanel>();session=GetComponent<MotionSession>();monitor=GetComponent<DeviceMonitor>();
        main.ControlToolbar.SetActive(false);main.ControlPanel.SetActive(false);main.MotionPanel.SetActive(false);
        main.SettingsPanel.SetActive(false);main.DevicePanel.SetActive(false);main.InformationPanel.SetActive(false);
        var old=transform.Find("Canvas");if(old!=null) {
            foreach(string name in new[]{"StatusPanel","RightPanelTab","OutputPanel"}){var t=old.Find(name);if(t!=null)t.gameObject.SetActive(false);}
        }
        session.InitializeViews();liveCamera=Camera.main;
        if(liveCamera!=null) {
            liveOrbit=liveCamera.GetComponent<CameraController>();
            replayCamera=new GameObject("ReplayCamera",typeof(Camera)).GetComponent<Camera>();replayCamera.CopyFrom(liveCamera);
            var liveRendering=liveCamera.GetComponent<UniversalAdditionalCameraData>();
            if(liveRendering!=null) {
                var replayRendering=replayCamera.gameObject.AddComponent<UniversalAdditionalCameraData>();
                replayRendering.renderPostProcessing=liveRendering.renderPostProcessing;
                replayRendering.volumeLayerMask=liveRendering.volumeLayerMask;
                replayRendering.volumeTrigger=replayCamera.transform;
                replayRendering.antialiasing=liveRendering.antialiasing;
                replayRendering.antialiasingQuality=liveRendering.antialiasingQuality;
                replayRendering.renderShadows=liveRendering.renderShadows;
            }
            replayCamera.transform.SetPositionAndRotation(liveCamera.transform.position,liveCamera.transform.rotation);
            replayOrbit=replayCamera.gameObject.AddComponent<CameraController>();replayOrbit.GUITransforms=new RectTransform[0];
            var focus=new GameObject("ReplayOrbitFocus");focus.transform.SetParent(replayCamera.transform,false);
            focus.transform.position=liveOrbit!=null&&liveOrbit.focusObject!=null?liveOrbit.focusObject.transform.position:Vector3.zero;
            // A fixed world pivot independent from the live model and live camera.
            focus.transform.SetParent(null,true);focus.transform.position=liveOrbit!=null&&liveOrbit.focusObject!=null?liveOrbit.focusObject.transform.position:Vector3.zero;
            replayOrbit.focusObject=focus;
            liveCamera.rect=replayCamera.rect=new Rect(.25f,.16f,.54f,.74f);
        }
        canvas=VisualizerUI.Canvas("VisualizerWorkspace",transform,100);
        var header=VisualizerUI.Box("Header",canvas.transform,new Color(.045f,.07f,.12f),0,.90f,1,1,true);
        VisualizerUI.Label("Brand",header.transform,"PN Link Visualizer",26,VisualizerUI.Ink,.023f,.25f,.27f,.91f);
        Label(header.transform,"Revision",VisualizerBuildInfo.Version+" · 单人动作测试",VisualizerBuildInfo.Version+" · Solo motion testing",13,VisualizerUI.Muted,.025f,.03f,.27f,.28f);
        string[] zh={"实时测试","录制回放","设备连接","更多工具"},en={"Live test","Record & replay","Connection","Tools"};
        for(int i=0;i<4;i++){int index=i;BButton("Tab"+i,header.transform,zh[i],en[i],()=>ShowTab(index),.285f+i*.145f,.21f,.422f+i*.145f,.79f);}
        BButton("Language",header.transform,"中文 / EN","EN / 中文",()=>VLoc.Toggle(),.88f,.21f,.985f,.79f);
        for(int i=0;i<4;i++) {
            pages[i]=VisualizerUI.Box("Page"+i,canvas.transform,new Color(.075f,.105f,.165f),0,.16f,.25f,.90f,true).gameObject;
            centers[i]=VisualizerUI.Box("Center"+i,canvas.transform,new Color(.065f,.09f,.145f),.25f,.16f,1,.90f,true).gameObject;
            footers[i]=VisualizerUI.Box("Footer"+i,canvas.transform,new Color(.045f,.07f,.12f),0,.065f,1,.16f,true).gameObject;
        }
        // The two 3D pages have separate avatars, cameras, orbit states and sensor views.
        centers[0].GetComponent<Image>().enabled=false;centers[1].GetComponent<Image>().enabled=false;
        liveStatus=DeviceStatusView.Create(centers[0].transform,()=>monitor.Capture(session.HasRecentData),false);
        replayStatus=DeviceStatusView.Create(centers[1].transform,()=>session.PlaybackStatus,true);
        // Status panels use screen-relative coordinates inside a full-width page root.
        SetGlobalStatusRect(liveStatus.transform);SetGlobalStatusRect(replayStatus.transform);
        BuildLive(pages[0].transform,footers[0].transform);BuildReplay(pages[1].transform,footers[1].transform);
        BuildConnection(pages[2].transform,centers[2].transform);BuildTools(pages[3].transform,centers[3].transform);
        var notifications=VisualizerUI.Box("Notifications",canvas.transform,new Color(.035f,.055f,.095f),0,0,1,.065f,true);
        status=VisualizerUI.Label("Status",notifications.transform,"",16,VisualizerUI.Muted,.025f,.1f,.98f,.9f);
        IsReady=true;ShowTab(main.pendingSettings?2:0);main.pendingSettings=false;
        if(message=="")ShowMessage("",false);
    }
    void SetGlobalStatusRect(Transform t)
    {
        var rect=(RectTransform)t;rect.anchorMin=new Vector2(.72f,0);rect.anchorMax=Vector2.one;rect.offsetMin=rect.offsetMax=Vector2.zero;
    }
    public static Text Label(Transform parent,string name,string zh,string en,int size,Color color,float l,float b,float r,float t)
    {
        var text=VisualizerUI.Label(name,parent,VLoc.Pick(zh,en),size,color,l,b,r,t);
        var label=text.gameObject.AddComponent<BilingualLabel>();label.Chinese=zh;label.English=en;return text;
    }
    public static Button BButton(string name,Transform parent,string zh,string en,Action action,float l,float b,float r,float t,bool primary=false)
    {
        var button=VisualizerUI.Button(name,parent,VLoc.Pick(zh,en),action,l,b,r,t,primary);
        var text=button.GetComponentInChildren<Text>();var label=text.gameObject.AddComponent<BilingualLabel>();label.Chinese=zh;label.English=en;return button;
    }
    Button Action(Transform parent,string name,string zh,string en,Action action,float row,bool primary=false)
    {
        var button=BButton(name,parent,zh,en,action,.08f,.69f-row*.085f,.92f,.763f-row*.085f,primary);deviceActions.Add(button);return button;
    }
    void BuildLive(Transform parent,Transform footer)
    {
        Label(parent,"Title","实时测试","Live test",28,VisualizerUI.Ink,.08f,.87f,.95f,.97f);
        connectionDot=VisualizerUI.Dot("ConnectionDot",parent,Color.gray,.08f,.815f,.125f,.84f);
        connectionState=VisualizerUI.Label("ConnectionState",parent,"",15,VisualizerUI.Muted,.16f,.79f,.94f,.86f);
        Action(parent,"Capture","1  开始采集","1  Start capture",main.startCapture,0);
        Action(parent,"Calibration","2  校准 A / T / F","2  Calibrate A / T / F",main.handleCalibration,1,true);
        Action(parent,"ResumeBody","恢复身体姿态","Restore body pose",main.handleResumeBody,2);
        Action(parent,"ResumeHands","恢复手部姿态","Restore hand pose",main.handleResumeHands,3);
        Action(parent,"ZeroPosition","位置归零","Zero position",main.handleZeroPosition,4);
        Action(parent,"StopCapture","停止采集","Stop capture",main.stopCapture,5);
        record=BButton("Record",parent,"开始本机录制 · 5 秒","Record locally · 5 s",()=>main.PrepareAction("开始本机录制","请准备动作，倒计时结束后开始记录。",session.StartRecording),.08f,.16f,.92f,.235f,true);
        stopRecord=BButton("StopRecord",parent,"结束录制并保存","Stop & save recording",()=>{main.CancelPreparation();session.StopRecording();RefreshFiles();},.08f,.07f,.92f,.145f);
        mode=VisualizerUI.Label("LiveMode",footer,"",20,VisualizerUI.Ink,.025f,.35f,.48f,.86f);
        Label(footer,"LiveHelp","摆姿势前留 5 秒 · F-Pose 末段站稳不动","5 seconds to prepare · stand still at the end of F-Pose",15,VisualizerUI.Muted,.025f,.05f,.73f,.4f);
        Label(footer,"CameraHelp","左键平移 · 右键旋转 · 滚轮缩放","Drag to pan · right-drag to orbit · scroll to zoom",14,VisualizerUI.Muted,.55f,.45f,.97f,.9f);
    }
    void BuildReplay(Transform parent,Transform footer)
    {
        Label(parent,"Title","录制回放","Record & replay",28,VisualizerUI.Ink,.08f,.87f,.94f,.97f);
        Label(parent,"Help","选择录像查看动作和当时的设备状态。","Open a take to inspect motion and recorded device status.",16,VisualizerUI.Muted,.08f,.75f,.92f,.87f);
        BButton("Folder",parent,"打开录制文件夹","Open recordings folder",OpenFolder,.08f,.66f,.92f,.735f);
        externalPath=Input(parent,"ExternalPath",.08f,.565f,.92f,.635f,"");
        BButton("OpenPath",parent,"打开填写的文件路径","Open file path",()=>session.LoadRecording(externalPath.text.Trim().Trim('"')),.08f,.48f,.92f,.55f);
        BButton("Refresh",parent,"刷新列表","Refresh list",RefreshFiles,.08f,.415f,.92f,.47f);
        library=VisualizerUI.Rect("Library",parent,.08f,.145f,.92f,.405f);
        BButton("Previous",parent,"上一页","Prev",()=>{fileIndex=Math.Max(0,fileIndex-1);RefreshFiles();},.08f,.09f,.36f,.135f);
        filePage=VisualizerUI.Label("PageNumber",parent,"",13,VisualizerUI.Muted,.39f,.09f,.61f,.135f,TextAnchor.MiddleCenter);
        BButton("Next",parent,"下一页","Next",()=>{++fileIndex;RefreshFiles();},.64f,.09f,.92f,.135f);
        BButton("CancelLoad",parent,"取消载入 / 关闭录像","Cancel load / close take",session.ReturnLive,.08f,.02f,.92f,.08f);
        play=VisualizerUI.Button("Play",footer,"播放",()=>session.TogglePlay(),.025f,.43f,.11f,.92f,true);playCaption=play.GetComponentInChildren<Text>();
        timeline=BuildSlider(footer,.13f,.57f,.76f,.77f);timeline.onValueChanged.AddListener(value=>session.Seek(value*session.Duration));
        clock=VisualizerUI.Label("Clock",footer,"",15,VisualizerUI.Muted,.77f,.43f,.92f,.94f,TextAnchor.MiddleCenter);
        var speed=VisualizerUI.Button("Speed",footer,"1×",CycleSpeed,.93f,.43f,.985f,.92f);speedCaption=speed.GetComponentInChildren<Text>();
        fileName=VisualizerUI.Label("TakeName",footer,"",15,VisualizerUI.Muted,.025f,.035f,.98f,.39f);
    }
    void BuildConnection(Transform left,Transform center)
    {
        Label(left,"Title","设备连接","Connection",28,VisualizerUI.Ink,.08f,.87f,.94f,.97f);
        Label(left,"Help","只在这里设置网络地址。\n采集和校准从实时测试开始。\n\n本机 IP 必须属于当前电脑，\n并与盒子上送地址保持一致。","Set network addresses here.\nStart capture from Live test.\n\nLocal IP must belong to this PC\nand match the device destination.",18,VisualizerUI.Muted,.08f,.30f,.92f,.82f);
        Label(center,"Title","网络与采集设置","Network & capture settings",30,VisualizerUI.Ink,.065f,.86f,.94f,.97f);
        Label(center,"Local","本机接收地址","Local receive endpoint",22,VisualizerUI.Ink,.065f,.73f,.94f,.82f);
        localIP=Input(center,"LocalIP",.065f,.62f,.66f,.72f,PlayerPrefs.GetString("UDP.Local.IP","127.0.0.1"));
        localPort=Input(center,"LocalPort",.70f,.62f,.94f,.72f,PlayerPrefs.GetInt("UDP.Local.Port",8002).ToString());
        Label(center,"Remote","设备命令地址","Device command endpoint",22,VisualizerUI.Ink,.065f,.49f,.94f,.58f);
        remoteIP=Input(center,"RemoteIP",.065f,.38f,.66f,.48f,PlayerPrefs.GetString("UDP.Remote.IP","127.0.0.1"));
        remotePort=Input(center,"RemotePort",.70f,.38f,.94f,.48f,PlayerPrefs.GetInt("UDP.Remote.Port",8080).ToString());
        ptp=PlayerPrefs.GetInt("StartCapture.EnablePTP",0)!=0;
        var ptpButton=VisualizerUI.Button("PTP",center,"",()=>ptp=!ptp,.065f,.24f,.60f,.33f);ptpCaption=ptpButton.GetComponentInChildren<Text>();
        apply=BButton("Apply",center,"保存并重新连接","Save & reconnect",()=>ApplySettings(),.63f,.24f,.94f,.33f,true);
        Label(center,"PortHelp","左侧填写 IPv4 地址，右侧填写 UDP 端口。默认接收 8002，命令端口 8080。\n修改不会清除已录制文件。","Enter an IPv4 address on the left and a UDP port on the right.\nDefaults: receive 8002, device commands 8080. Recordings are preserved.",17,VisualizerUI.Muted,.065f,.07f,.94f,.21f);
    }
    void BuildTools(Transform left,Transform center)
    {
        Label(left,"Title","更多工具","Tools",28,VisualizerUI.Ink,.08f,.87f,.94f,.97f);
        Action(left,"ResetDrift","重置零漂 · 5 秒","Reset zero drift · 5 s",main.handleReset0MotionDrift,0);
        BButton("CopyLogs",left,"复制运行日志","Copy log",()=>GUIUtility.systemCopyBuffer=string.Join("\n",messages.Select(VLoc.T).ToArray()),.08f,.55f,.92f,.63f);
        Label(left,"FormatInfo",".mocap 是本工具的动作记录。\n可反复查看骨骼动作；\n不包含原始 IMU 数据。\n\n"+VisualizerBuildInfo.Version+" 同时保存历史设备状态。\nr7 录像仍然可以打开。",".mocap is this tool's pose format.\nIt stores skeletal motion,\nnot raw IMU data.\n\n"+VisualizerBuildInfo.Version+" includes device snapshots.\nr7 recordings remain readable.",17,VisualizerUI.Muted,.08f,.10f,.92f,.48f);
        Label(center,"Title","运行日志与版本","Log & version",28,VisualizerUI.Ink,.065f,.86f,.94f,.97f);
        info=VisualizerUI.Label("Version",center,"",18,VisualizerUI.Muted,.065f,.71f,.94f,.85f);
        RectTransform content;var root=DeviceStatusView.BuildScroll(center,"Logs",.065f,.06f,.94f,.69f,out content);
        content.sizeDelta=new Vector2(0,1300);
        logs=VisualizerUI.Label("LogText",root,"",17,VisualizerUI.Ink,.02f,0,.98f,1,TextAnchor.UpperLeft);
    }
    public static InputField Input(Transform parent,string name,float l,float b,float r,float t,string value)
    {
        var box=VisualizerUI.Box(name,parent,new Color(.15f,.21f,.30f),l,b,r,t,true);var field=box.gameObject.AddComponent<InputField>();
        field.textComponent=VisualizerUI.Label("Text",box.transform,"",23,VisualizerUI.Ink,.04f,0,.96f,1);
        field.textComponent.GetComponent<LocalizedText>().enabled=false;field.targetGraphic=box;field.characterLimit=2048;field.text=value;return field;
    }
    public bool ApplySettings()
    {
        if(session.IsRecording||main.IsCalibrating||GetComponent<MocapApplication>().HasPendingCommands||(main.ActionPrompt!=null&&main.ActionPrompt.IsPending)) {
            ShowMessage(VLoc.Pick("请先结束录制或设备操作，再修改连接。","Finish recording or device actions before reconnecting."),true);return false;
        }
        int lp,rp;string issue;
        if(!int.TryParse(localPort.text.Trim(),out lp)||!int.TryParse(remotePort.text.Trim(),out rp)){ShowMessage("UDP 端口必须是 1 到 65535 的整数。",true);return false;}
        if(!MocapConnectionSettings.Validate(localIP.text.Trim(),lp,remoteIP.text.Trim(),rp,out issue)){ShowMessage(issue,true);return false;}
        PlayerPrefs.SetString("UDP.Local.IP",localIP.text.Trim());PlayerPrefs.SetInt("UDP.Local.Port",lp);
        PlayerPrefs.SetString("UDP.Remote.IP",remoteIP.text.Trim());PlayerPrefs.SetInt("UDP.Remote.Port",rp);
        PlayerPrefs.SetInt("StartCapture.EnablePTP",ptp?1:0);PlayerPrefs.Save();GetComponent<MocapApplication>().reset();ShowMessage("设备配置已保存。",false);return true;
    }
    public void ShowTab(int index)
    {
        if(!IsReady){main.pendingSettings=index==2;return;}
        SelectedTab=index;
        for(int i=0;i<4;i++){pages[i].SetActive(i==index);centers[i].SetActive(i==index);footers[i].SetActive(i==index);}
        footers[2].SetActive(false);footers[3].SetActive(false);
        // Forms and logs get the whole height; they have no motion stage or transport.
        for(int i=2;i<4;i++){var rt=(RectTransform)centers[i].transform;rt.anchorMin=new Vector2(.25f,.065f);var ls=(RectTransform)pages[i].transform;ls.anchorMin=new Vector2(0,.065f);}
        session.SetPage(index);
        if(liveCamera!=null){liveCamera.enabled=index==0;replayCamera.enabled=index==1;}
        if(liveOrbit!=null)liveOrbit.enabled=index==0;if(replayOrbit!=null)replayOrbit.enabled=index==1;
        var header=canvas.transform.Find("Header");for(int i=0;i<4;i++)header.Find("Tab"+i).GetComponent<Image>().color=i==index?VisualizerUI.Blue:new Color(.16f,.21f,.30f);
        if(index==1)RefreshFiles();nextRefresh=0;
    }
    public void ShowSensors(){if(SelectedTab>1)ShowTab(0);}
    public void ShowMessage(string text,bool isError)
    {
        message=text;error=isError;if(!string.IsNullOrEmpty(text)){messages.Enqueue(text);while(messages.Count>120)messages.Dequeue();}
    }
    public void RefreshFiles()
    {
        if(library==null)return;
        foreach(Transform child in library){child.gameObject.SetActive(false);Destroy(child.gameObject);}
        try {
            string[] files=Directory.Exists(session.DirectoryPath)?Directory.GetFiles(session.DirectoryPath).Where(f=>f.EndsWith(".mocap",StringComparison.OrdinalIgnoreCase)||f.EndsWith(".mocap.partial",StringComparison.OrdinalIgnoreCase)).OrderByDescending(f=>f,StringComparer.Ordinal).ToArray():new string[0];
            fileIndex=Math.Max(0,Math.Min(fileIndex,Math.Max(0,(files.Length-1)/4)));filePage.text=(fileIndex+1)+" / "+Math.Max(1,(files.Length+3)/4);
            if(files.Length==0)Label(library,"Empty","还没有录像。\n在实时测试中录制后会出现在这里。","No recordings yet.\nRecord a take in Live test first.",16,VisualizerUI.Muted,0,0,1,1);
            for(int i=0;i<4&&fileIndex*4+i<files.Length;i++) {string path=files[fileIndex*4+i];VisualizerUI.Button("Take"+i,library,Path.GetFileName(path),()=>session.LoadRecording(path),0,.77f-i*.25f,1,.98f-i*.25f);}
        }catch(Exception e){ShowMessage("无法读取录像目录："+e.Message,true);}
    }
    void OpenFolder(){try{Directory.CreateDirectory(session.DirectoryPath);Application.OpenURL(new Uri(session.DirectoryPath+Path.DirectorySeparatorChar).AbsoluteUri);}catch(Exception e){ShowMessage("无法打开目录："+e.Message,true);}}
    void CycleSpeed(){float[] rates={.25f,.5f,1,2};session.Speed=rates[(Array.IndexOf(rates,session.Speed)+1)%rates.Length];speedCaption.text=session.Speed+"×";}
    static string Seconds(double seconds){var t=TimeSpan.FromSeconds(seconds);return ((int)t.TotalMinutes).ToString("00")+":"+t.Seconds.ToString("00")+"."+(t.Milliseconds/100);}
    void Update()
    {
        if(!IsReady||Time.unscaledTime<nextRefresh)return;nextRefresh=Time.unscaledTime+.1f;
#if UNITY_STANDALONE_WIN && !UNITY_EDITOR
        try{if(!titleSet){uint own=(uint)System.Diagnostics.Process.GetCurrentProcess().Id;EnumWindows((hwnd,arg)=>{uint pid;GetWindowThreadProcessId(hwnd,out pid);if(pid==own&&IsWindowVisible(hwnd)){titleSet=SetWindowText(hwnd,"PN Link Visualizer");if(titleSet)return false;}return true;},IntPtr.Zero);}}catch(Exception){}
#endif
        bool busy=main.IsCalibrating||(main.ActionPrompt!=null&&main.ActionPrompt.IsPending)||GetComponent<MocapApplication>().HasPendingCommands;
        foreach(var b in deviceActions)b.interactable=SelectedTab!=1&&(!busy||b.name=="Calibration");
        record.interactable=!session.IsRecording&&!busy&&session.HasRecentData;stopRecord.interactable=session.IsRecording||(main.ActionPrompt!=null&&main.ActionPrompt.IsPending);
        play.interactable=session.IsPlayback&&!session.IsLoading;timeline.interactable=play.interactable;
        playCaption.text=session.Playing?"暂停":"播放";
        timeline.SetValueWithoutNotify(session.Duration>0?(float)(session.Position/session.Duration):0);clock.text=Seconds(session.Position)+" / "+Seconds(session.Duration);
        connectionDot.color=session.HasRecentData?new Color(.18f,.86f,.48f):Color.gray;
        connectionState.text=VLoc.Pick(session.HasRecentData?"实时数据已接通":"等待实时数据",session.HasRecentData?"Live data connected":"Waiting for live data");
        mode.text=session.IsRecording?VLoc.Pick("● 正在录制  ","● Recording  ")+Seconds(session.RecordingSeconds):main.CalibrationPhase==CalibrationGuidePhase.Processing?"校准处理中 · 可自由活动":main.IsCalibrating?"正在校准 · 按大窗口提示操作":session.HasRecentData?"实时 · 正在接收动作":"实时 · 等待动作数据";
        mode.color=session.IsRecording?new Color(1,.4f,.4f):VisualizerUI.Ink;
        fileName.text=session.IsLoading?VLoc.Pick("正在载入录像…","Loading recording…"):string.IsNullOrEmpty(session.CurrentFile)?VLoc.Pick("尚未选择录像","No recording selected"):Path.GetFileName(session.CurrentFile)+(session.IsPlayback?" · "+(session.Playing?VLoc.Pick("播放中","playing"):VLoc.Pick("已暂停","paused")):"");
        ptpCaption.text=VLoc.Pick("PTP 时间同步：","PTP time sync: ")+(ptp?VLoc.Pick("开","on"):VLoc.Pick("关","off"));
        status.text=string.IsNullOrEmpty(message)?VLoc.Pick("在实时测试中采集、校准和录制，在录制回放中查看录像。","Capture, calibrate and record in Live test. Inspect takes in Record & replay."):VLoc.T(message);status.color=error?new Color(1,.72f,.4f):VisualizerUI.Muted;
        logs.text=string.Join("\n\n",messages.Select(VLoc.T).ToArray());
        info.text=VisualizerBuildInfo.Product+" "+VisualizerBuildInfo.Version+" (build "+VisualizerBuildInfo.Build+")\nMocapApi "+MocapApi.Version.libMocapApiString+" · "+VLoc.Pick("记录格式：NVPOSE2（兼容 NVPOSE1）","Recording: NVPOSE2 (reads NVPOSE1)");
    }
    static Slider BuildSlider(Transform parent,float l,float b,float r,float t)
    {
        var root=VisualizerUI.Box("Timeline",parent,new Color(.23f,.29f,.39f),l,b,r,t,true);var slider=root.gameObject.AddComponent<Slider>();slider.minValue=0;slider.maxValue=1;
        var fill=VisualizerUI.Box("Fill",root.transform,VisualizerUI.Blue,0,0,1,1);var handle=VisualizerUI.Box("Handle",root.transform,Color.white,0,-.4f,0,1.4f,true);handle.rectTransform.sizeDelta=new Vector2(12,0);
        slider.fillRect=fill.rectTransform;slider.handleRect=handle.rectTransform;slider.targetGraphic=handle;return slider;
    }
    void OnDestroy(){if(replayOrbit!=null&&replayOrbit.focusObject!=null)Destroy(replayOrbit.focusObject);if(replayCamera!=null)Destroy(replayCamera.gameObject);}
}
