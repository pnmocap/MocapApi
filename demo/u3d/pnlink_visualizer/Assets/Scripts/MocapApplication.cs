using UnityEngine;
using MocapApi;
using System.Runtime.InteropServices;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using Unity.VisualScripting;

public class MocapApplication : MonoBehaviour
{
    ulong applicationHandle = 0;
    bool connectionOpen;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    public GameObject avatarGameObject;
    Dictionary<ulong, AvatarController> avatarControllerMap = new Dictionary<ulong, AvatarController>();
    Dictionary<ulong, AbstractCommandProcess> pendingCommandMap = new Dictionary<ulong, AbstractCommandProcess>();
    bool _masterLive = false;
    public bool HasPendingCommands { get { return pendingCommandMap.Count != 0; } }
    readonly MCPEvent_t[] eventBuffer = new MCPEvent_t[64];
    bool eventBufferReady;
    public MainPanel MainPanel;

    void Start()
    {
#if !UNITY_EDITOR
    initMCPApplication();
#endif
    }
    private void OnDestroy()
    {
#if !UNITY_EDITOR
    uninitMCPApplication();
#endif
    }
    void initMCPApplication(bool useSavedRotation = false)
    {
        string localIP = PlayerPrefs.GetString("UDP.Local.IP", "127.0.0.1").Trim();
        int localPort = PlayerPrefs.GetInt("UDP.Local.Port", 8002);
        string remoteIP = PlayerPrefs.GetString("UDP.Remote.IP", "127.0.0.1").Trim();
        int remotePort = PlayerPrefs.GetInt("UDP.Remote.Port", 8080);
        string error;
        if (!MocapConnectionSettings.Validate(localIP, localPort, remoteIP, remotePort, out error) ||
            !MocapConnectionSettings.CanBind(localIP, localPort, out error))
        {
            ReportConnectionError(error);
            return;
        }
        try
        {
            Require(IMCPApplication.Application.CreateApplication(ref applicationHandle), "CreateApplication");
            if (applicationHandle == 0) throw new System.InvalidOperationException("CreateApplication returned a zero handle.");
            ulong settings = 0;
            try
            {
                Require(IMCPSettings.Settings.CreateSettings(ref settings), "CreateSettings");
                Require(IMCPSettings.Settings.SetSettingsBvhData(EMCPBvhData.BvhDataType_Binary, settings), "SetSettingsBvhData");
                var rotation = useSavedRotation
                    ? (EMCPBvhRotation)PlayerPrefs.GetInt("Bvh.Rotation.Order", (int)EMCPBvhRotation.BvhRotation_YZX)
                    : EMCPBvhRotation.BvhRotation_YXZ;
                Require(IMCPSettings.Settings.SetSettingsBvhRotation(rotation, settings), "SetSettingsBvhRotation");
                Require(IMCPSettings.Settings.SetSettingsBvhTransformation(EMCPBvhTransformation.BvhTransformation_Enable, settings), "SetSettingsBvhTransformation");
                Require(IMCPSettings.Settings.SetSettingsUDPEx(localIP, (ushort)localPort, settings), "SetSettingsUDPEx");
                Require(IMCPSettings.Settings.SetSettingsUDPServer(remoteIP, (ushort)remotePort, settings), "SetSettingsUDPServer");
                Require(IMCPApplication.Application.SetApplicationSettings(settings, applicationHandle), "SetApplicationSettings");
            }
            finally
            {
                if (settings != 0) IMCPSettings.Settings.DestroySettings(settings);
            }
            Require(IMCPApplication.Application.OpenApplication(applicationHandle), "OpenApplication");
            connectionOpen = true;
            ulong renderSettings = 0;
            Require(IMCPRenderSettings.RenderSettings.GetPreDefRenderSettings(EMCPPreDefinedRenderSettings.PreDefinedRenderSettings_Unity3D, ref renderSettings), "GetPreDefRenderSettings");
            Require(IMCPApplication.Application.SetApplicationRenderSettings(renderSettings, applicationHandle), "SetApplicationRenderSettings");
            Debug.Log($"Visualizer UDP ready: {localIP}:{localPort}; device {remoteIP}:{remotePort}.");
        }
        catch (System.Exception e)
        {
            uninitMCPApplication();
            ReportConnectionError("设备连接初始化失败：" + e.Message);
        }
    }

    static void Require(EMCPError error, string operation)
    {
        if (error != EMCPError.Error_None)
            throw new System.InvalidOperationException(operation + ": " + error);
    }

    void ReportConnectionError(string message)
    {
        Debug.LogWarning("Visualizer connection unavailable: " + message);
        if (MainPanel == null) return;
        MainPanel.ReportError(message);
        MainPanel.handleSettings();
    }

    public void reset()
    {
        if (MainPanel != null) MainPanel.CancelPreparation();
        var session=GetComponent<MotionSession>();
        if(session != null) session.StopRecording();
        uninitMCPApplication();
        initMCPApplication(true);
    }
    void uninitMCPApplication()
    {
        if (applicationHandle == 0) return;
        InterruptPendingCommands("连接已关闭，无法继续确认本次校准状态。");
        if (connectionOpen) IMCPApplication.Application.CloseApplication(applicationHandle);
        connectionOpen = false;
        avatarControllerMap.Clear();
        _masterLive = false;
        var monitor=GetComponent<DeviceMonitor>(); if(monitor!=null)monitor.Clear();
        IMCPApplication.Application.DestroyApplication(applicationHandle);
        applicationHandle = 0;
    }

#if UNITY_EDITOR
    private void OnDisable()
    {
        uninitMCPApplication();
    }
    private void OnEnable()
    {
        initMCPApplication();
    }
#endif

    void InterruptPendingCommands(string reason)
    {
        foreach (var process in pendingCommandMap.Values) process.handleInterrupted(reason);
        pendingCommandMap.Clear();
    }

    public bool queueCommand(ulong commandHandle, AbstractCommandProcess commandProcess_)
    {
        var session = GetComponent<MotionSession>();
        if (session != null && (session.PlaybackVisible || session.IsLoading)) {
            commandProcess_.handleInterrupted("回放中不发送设备命令。");
            commandProcess_.DestroyCommand();
            MainPanel.ReportWarning("请先返回实时，再操作设备。"); return false;
        }
        if (!connectionOpen || applicationHandle == 0 || commandHandle == 0)
        {
            commandProcess_.DestroyCommand();
            if (MainPanel != null) MainPanel.ReportError("设备连接尚未准备好，命令未发送。");
            return false;
        }
        pendingCommandMap.Add(commandHandle, commandProcess_);
        var error = IMCPApplication.Application.QueuedServerCommand(commandHandle, applicationHandle);
        if (error != EMCPError.Error_None)
        {
            pendingCommandMap.Remove(commandHandle);
            commandProcess_.DestroyCommand();
            if (MainPanel != null) MainPanel.ReportError($"Queue command failed: {error}.");
            return false;
        }
        if (pendingCommandMap.Count > 1)
        {
            MainPanel.ReportError($"Pending command {pendingCommandMap.Count} is running.");
        }
        return true;
    }
    // Update is called once per frame
    void Update()
    {
        if (!connectionOpen || applicationHandle == 0) return;
        if (!eventBufferReady) {
            for(int i=0;i<eventBuffer.Length;i++) eventBuffer[i].size=(uint)Marshal.SizeOf<MCPEvent_t>();
            eventBufferReady=true;
        }
        // Drain bursts without starving the UI. Reuse the array instead of allocating every render frame.
        for(int batch=0;batch<8;batch++) {
            uint count=(uint)eventBuffer.Length;
            var er=IMCPApplication.Application.PollApplicationNextEvent(eventBuffer,ref count,applicationHandle);
            if(er==EMCPError.Error_None || er==EMCPError.Error_MoreEvent) DispatchEvents(eventBuffer,count);
            if(er!=EMCPError.Error_MoreEvent || count==0) break;
        }
    }

    internal void DispatchEvents(MCPEvent_t[] ev, uint count)
    {
        int available = (int)System.Math.Min(count, (uint)ev.Length);
        for (int eventIndex = 0; eventIndex < available; ++eventIndex)
        {
            switch (ev[eventIndex].eventType)
            {
                case EMCPEventType.MCPEvent_AvatarUpdated:
                    updateAvatarMotion(ev[eventIndex].eventData.motionData.avatarHandle);
                    break;
                case EMCPEventType.MCPEvent_Notify:
                    handleNotify(ev[eventIndex].eventData.notifyData);
                    break;
                case EMCPEventType.MCPEvent_Error:
                    Debug.LogErrorFormat("Application open failed : {0}", ev[eventIndex].eventData.systemError.error);
                    break;
                case EMCPEventType.MCPEvent_CommandReply:
                    if (pendingCommandMap.ContainsKey(ev[eventIndex].eventData.commandRespond._commandHandle))
                    {
                        switch (ev[eventIndex].eventData.commandRespond._replay)
                        {
                            case EMCPReplay.MCPReplay_Response:
                                pendingCommandMap[ev[eventIndex].eventData.commandRespond._commandHandle].handleResponse();
                                break;
                            case EMCPReplay.MCPReplay_Running:
                                pendingCommandMap[ev[eventIndex].eventData.commandRespond._commandHandle].handleRunning();
                                break;
                            case EMCPReplay.MCPReplay_Result:
                                pendingCommandMap[ev[eventIndex].eventData.commandRespond._commandHandle].handleResult();
                                pendingCommandMap.Remove(ev[eventIndex].eventData.commandRespond._commandHandle);
                                break;
                            default:
                                Debug.LogWarning("Unknown command reply.");
                                break;
                        }
                    }
                    else {
                        Debug.LogErrorFormat("Unmanaged command.");
                    }
                    break;

                default:
                    break;
            }
        }
    }
    void handleNotify(MCPEvent_NotifyData_t notifyData)
    {
        switch (notifyData._notify)
        {
            case EMCPNotify.Notify_SystemUpdated:
                MainPanel.updateSystem(notifyData._notifyHandle);
                if (!_masterLive)
                {
                    EMCPMasterStatus masterStatus = MocapApi.EMCPMasterStatus.MasterStatus_Offline;
                    IMCPSystem.System.GetMasterStatus(ref masterStatus, notifyData._notifyHandle);
                    if (masterStatus == EMCPMasterStatus.MasterStatus_Online)
                    {
                        MainPanel.ReportInfo("Master Online.");
                        _masterLive = true;
                    }
                }
                else
                {
                    EMCPMasterStatus masterStatus = MocapApi.EMCPMasterStatus.MasterStatus_Online;
                    IMCPSystem.System.GetMasterStatus(ref masterStatus, notifyData._notifyHandle);
                    if (masterStatus == EMCPMasterStatus.MasterStatus_Offline)
                    {
                        MainPanel.ReportInfo("Master Offline.");
                        _masterLive = false;
                    }
                }
                break;
            default:
                Debug.LogErrorFormat("Unhandled notify : {0}", notifyData._notify);
                break;
        }
    }

    void updateAvatarMotion(ulong avatarHandle)
    {
        var session=GetComponent<MotionSession>();

        if (avatarControllerMap.ContainsKey(avatarHandle))
        {
            avatarControllerMap[avatarHandle].updateMotion();
        }
        else
        {
            Debug.Assert(null != avatarGameObject);
            avatarControllerMap[avatarHandle] = avatarGameObject.GetComponent<AvatarController>();
            avatarControllerMap[avatarHandle].attachAvatar(avatarHandle);
        }
        if (session != null) session.NotifyLiveFrame();
    }
}
