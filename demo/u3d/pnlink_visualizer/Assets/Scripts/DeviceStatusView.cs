using System;
using UnityEngine;
using UnityEngine.UI;

// Each page owns its own instance and source; replay never queries the live SDK.
public sealed class DeviceStatusView : MonoBehaviour
{
    Func<DeviceSnapshot> source;
    bool recorded;
    Text summary,heading;
    Image dot;
    Transform rows;
    RectTransform content;
    readonly System.Collections.Generic.List<Text> labels=new System.Collections.Generic.List<Text>();
    DeviceSnapshot snapshot;
    Canvas details;
    Text detailText;
    uint? selected;
    float nextRefresh;
    public static DeviceStatusView Create(Transform parent,Func<DeviceSnapshot> source,bool recorded)
    {
        var area=VisualizerUI.Box(recorded?"ReplayDeviceStatus":"LiveDeviceStatus",parent,new Color(.075f,.105f,.165f),.79f,.16f,1,.90f,true);
        var view=area.gameObject.AddComponent<DeviceStatusView>();view.source=source;view.recorded=recorded;
        view.heading=VisualizerUI.Label("Heading",area.transform,"",21,VisualizerUI.Ink,.08f,.89f,.95f,.98f);
        view.dot=VisualizerUI.Dot("ConnectionDot",area.transform,Color.gray,.08f,.83f,.12f,.855f);
        view.summary=VisualizerUI.Label("Summary",area.transform,"",14,VisualizerUI.Muted,.08f,.67f,.94f,.82f);
        view.rows=BuildScroll(area.transform,"Sensors",.05f,.035f,.95f,.66f,out view.content);
        return view;
    }
    public static Transform BuildScroll(Transform parent,string name,float l,float b,float r,float t,out RectTransform content)
    {
        var root=VisualizerUI.Box(name,parent,new Color(.07f,.10f,.16f),l,b,r,t,true);
        var scroll=root.gameObject.AddComponent<ScrollRect>();scroll.horizontal=false;scroll.movementType=ScrollRect.MovementType.Clamped;
        var viewport=VisualizerUI.Rect("Viewport",root.transform,0,0,1,1);viewport.gameObject.AddComponent<RectMask2D>();
        content=VisualizerUI.Rect("Content",viewport,0,1,1,1);content.pivot=new Vector2(.5f,1);content.sizeDelta=new Vector2(0,0);
        var barImage=VisualizerUI.Box("Scrollbar",root.transform,new Color(.13f,.18f,.26f),.96f,0,1,1,true);
        var bar=barImage.gameObject.AddComponent<Scrollbar>();
        var handle=VisualizerUI.Box("Handle",barImage.transform,new Color(.40f,.49f,.63f),0,0,1,1,true);
        bar.handleRect=handle.rectTransform;bar.targetGraphic=handle;bar.direction=Scrollbar.Direction.BottomToTop;
        viewport.anchorMax=new Vector2(.95f,1);scroll.verticalScrollbar=bar;scroll.verticalScrollbarVisibility=ScrollRect.ScrollbarVisibility.AutoHide;
        scroll.viewport=viewport;scroll.content=content;scroll.scrollSensitivity=24;return content;
    }
    void Update()
    {
        if(Time.unscaledTime<nextRefresh)return;nextRefresh=Time.unscaledTime+.2f;
        snapshot=source();heading.text=VLoc.Pick(recorded?"录像中的设备状态":"设备与传感器",recorded?"Recorded device status":"Device & sensors");
        bool available=snapshot!=null&&snapshot.Available;
        bool connected=snapshot!=null&&(snapshot.Motion||snapshot.Online);
        dot.color=connected?new Color(.18f,.86f,.48f):new Color(.48f,.54f,.63f);
        if(snapshot==null)summary.text=VLoc.Pick(recorded?"此录像没有设备状态。\nr7 录像只保存骨骼动作。":"尚未收到设备状态。",recorded?"No recorded device status.\nr7 takes contain poses only.":"No device status received.");
        else summary.text=VLoc.Pick(recorded?"历史快照 · 随时间轴变化":"实时快照 · 自动更新",recorded?"Historical · follows timeline":"Live · updates automatically")+"\n"+
            VLoc.Pick("动作数据：","Motion: ")+(snapshot.Motion?VLoc.Pick("已接通","receiving"):VLoc.Pick("暂无","none"))+"\n"+
            (available?("SN "+snapshot.Serial+"\n"+VLoc.Pick("传感器：","Sensors: ")+snapshot.Sensors.Length):VLoc.Pick("未收到设备详细信息","Device details unavailable"));
        int count=available?snapshot.Sensors.Length:0;
        while(labels.Count<count) {
            int index=labels.Count;
            var b=VisualizerUI.Button("Sensor"+index,rows,"",()=>ShowDetail(index),0,1,1,1);
            var rect=(RectTransform)b.transform;rect.pivot=new Vector2(.5f,1);rect.anchoredPosition=new Vector2(0,-index*59);rect.sizeDelta=new Vector2(0,54);
            labels.Add(b.GetComponentInChildren<Text>());
        }
        for(int i=0;i<labels.Count;i++) {
            labels[i].transform.parent.gameObject.SetActive(i<count);
            if(i>=count)continue;
            var s=snapshot.Sensors[i];
            labels[i].text=(string.IsNullOrEmpty(s.Bone)?"#"+s.Id:s.Bone)+"\n"+
                VLoc.Pick(connected?"接收 ":"上次接收 ",connected?"RX ":"Last RX ")+(s.PacketRate<0?"—":s.PacketRate.ToString("0.0")+"%")+"  ·  "+Mag(s.Magnetic);
            labels[i].fontSize=15;labels[i].resizeTextMaxSize=15;
        }
        content.sizeDelta=new Vector2(0,count*59);
        if(details!=null && details.gameObject.activeSelf && selected.HasValue) {
            var s=Array.Find(snapshot==null?new SensorSnapshot[0]:snapshot.Sensors,x=>x.Id==selected.Value);
            if(s==null)detailText.text=VLoc.Pick("当前帧无此传感器状态。","No status for this sensor at the current frame.");
            else detailText.text=VLoc.Pick(recorded?"来源：录像历史帧":"来源：实时设备数据",recorded?"Source: recorded frame":"Source: live device")+"\n\n"+
                "ID: "+s.Id+"\n"+VLoc.Pick("绑定骨骼：","Bone: ")+s.Bone+"\n"+VLoc.Pick("序列号：","Serial: ")+s.Serial+"\n"+
                VLoc.Pick("固件版本：","Firmware: ")+s.Firmware+"\n"+VLoc.Pick("接收率：","Packet reception: ")+(s.PacketRate<0?"—":s.PacketRate.ToString("0.0")+"%")+"\n"+
                VLoc.Pick("磁环境：","Magnetic environment: ")+Mag(s.Magnetic)+"\n\n"+
                VLoc.Pick("接收率为设备报告的包接收比例，不是算法帧率。","Packet reception is the device-reported ratio, not the algorithm frame rate.");
        }
    }
    static string Mag(int mag) { return mag==3?VLoc.Pick("磁良好","Mag good"):mag==2?VLoc.Pick("磁较差","Mag poor"):mag==1?VLoc.Pick("磁很差","Mag bad"):mag==0?VLoc.Pick("磁极差","Mag worst"):VLoc.Pick("磁未知","Mag unknown"); }
    void ShowDetail(int index)
    {
        if(snapshot==null||index>=snapshot.Sensors.Length)return; selected=snapshot.Sensors[index].Id;
        if(details==null) {
            var owner=GetComponentInParent<MainPanel>();
            details=VisualizerUI.Canvas("SensorDetails",owner!=null?owner.transform:transform.root,29000);
            var shade=VisualizerUI.Box("Shade",details.transform,new Color(.015f,.025f,.05f,.85f),0,0,1,1,true);
            var card=VisualizerUI.Box("Card",shade.transform,new Color(.085f,.12f,.19f),.20f,.12f,.80f,.88f,true);
            detailText=VisualizerUI.Label("Details",card.transform,"",24,VisualizerUI.Ink,.07f,.17f,.93f,.94f);
            VisualizerWorkspace.BButton("Close",card.transform,"关闭","Close",()=>details.gameObject.SetActive(false),.64f,.05f,.93f,.14f);
        }
        details.gameObject.SetActive(true);nextRefresh=0;
    }
    public void HideDetail() { if(details!=null)details.gameObject.SetActive(false); }
    void OnDisable() { HideDetail(); }
    void OnDestroy() { if(details!=null)Destroy(details.gameObject); }
}
