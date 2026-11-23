using UnityEngine;
using TMPro;
using MocapApi;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System;
using UnityEngine.UI;
using System.Runtime.InteropServices;

public class MainPanel : MonoBehaviour, AbstractReporter
{
    public GameObject ControlPanel;
    public GameObject ControlToolbar;
    public GameObject MotionPanel;
    public GameObject SettingsPanel;
    public GameObject InformationPanel;
    OutputPanel _outputPanel;
    public GameObject DevicePanel;
    StatusPanel _statusPanel;
    public GameObject GPSTrackerPanel;
    CalibrationProcess calibrationProcess;
    CalibrationGuidePanel calibrationGuide;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        SettingsPanel.GetComponent<Settings>().setAppliedDelegate(() =>
        {
            GetComponent<MocapApplication>().reset();
        });
        if (GetComponent<DeviceMonitor>() == null) gameObject.AddComponent<DeviceMonitor>();
        if (GetComponent<MotionSession>() == null) gameObject.AddComponent<MotionSession>();
        Workspace = gameObject.AddComponent<VisualizerWorkspace>();
    }

    // Update is called once per frame
    [NonSerialized] public bool pendingSettings;
    public SoloActionPrompt ActionPrompt { get; private set; }
    public VisualizerWorkspace Workspace { get; private set; }
    public bool IsCalibrating { get { return calibrationProcess != null && !calibrationProcess.isComplete(); } }
    public CalibrationGuidePhase CalibrationPhase { get { return calibrationProcess == null ? CalibrationGuidePhase.Waiting : calibrationProcess.Guide.Phase; } }
    CalibrationGuidePhase lastCalibrationPhase = CalibrationGuidePhase.Waiting;
    bool lastStandingFinish;
    public void CancelPreparation() { if (ActionPrompt != null) ActionPrompt.Cancel(); }
    public void PrepareAction(string title, string instruction, Action action)
    {
        var session = GetComponent<MotionSession>();
        if (session != null && (session.PlaybackVisible || session.IsLoading)) { ReportWarning("请先返回实时，再发送设备操作。"); return; }
        if (IsCalibrating) { ReportWarning("校准正在进行，请先等待结果。"); return; }
        if (GetComponent<MocapApplication>().HasPendingCommands) { ReportWarning("设备正在执行其他操作，请稍后重试。"); return; }
        if (ActionPrompt == null) ActionPrompt = SoloActionPrompt.Create(transform);
        ActionPrompt.Begin(title, instruction, action);
    }
    void Update()
    {
        bool standing=calibrationProcess!=null&&calibrationProcess.Guide.IsStandingFinish;
        if(standing&&!lastStandingFinish) {
            ReportInfo("请停止走路，双脚站稳，双臂自然下垂，保持不动直到采集结束。");
            if(calibrationGuide==null||!calibrationGuide.gameObject.activeInHierarchy)VisualizerFeedback.Play(gameObject,true);
        }
        lastStandingFinish=standing;
        var phase=CalibrationPhase;
        if(phase==lastCalibrationPhase)return;
        lastCalibrationPhase=phase;
        if(phase==CalibrationGuidePhase.Processing || phase==CalibrationGuidePhase.Succeeded) {
            ReportInfo(phase==CalibrationGuidePhase.Processing?"采集结束，可以自由活动。正在处理校准数据。":"校准完成，可以开始动作测试。");
            if(calibrationGuide==null || !calibrationGuide.gameObject.activeInHierarchy) VisualizerFeedback.Play(gameObject,true);
        }
    }

    public void updateSystem(ulong systemHandle)
    {
        if(GetComponent<DeviceMonitor>()==null)gameObject.AddComponent<DeviceMonitor>();
        GetComponent<DeviceMonitor>().Read(systemHandle);
    }
    public void startCaptureWithPtp() { PrepareAction("开始采集（PTP）", "请走到测试位置。", startCaptureWithPtpNow); }
    void startCaptureWithPtpNow()
    {
        var gpsController = GetComponent<GPSController>();
        if (null != gpsController)
        {
            gpsController.StartUpdateGPS();
        }
        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandStartCapture, ref pendingCommand);
        IMCPCommand.Command.SetCommandExtraLong((uint)EMCPCommandExtraLong.CommandExtraLong_Extra1, new IntPtr((uint)EStartCaptureFlag.StartCaptureFlag_EnablePtp), pendingCommand);
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "Start Capture", this));
    }
    public void startCapture() { PrepareAction("开始采集", "请走到测试位置，倒计时结束后开始采集。", startCaptureNow); }
    void startCaptureNow()
    {
        var gpsController = GetComponent<GPSController>();
        if (null != gpsController)
        {
            gpsController.StartUpdateGPS();
        }
        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandStartCapture, ref pendingCommand);
        if (PlayerPrefs.GetInt("StartCapture.EnablePTP", 0) != 0) {
            IMCPCommand.Command.SetCommandExtraLong((uint)EMCPCommandExtraLong.CommandExtraLong_Extra1, new IntPtr((uint)EStartCaptureFlag.StartCaptureFlag_EnablePtp), pendingCommand);
        }
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "Start Capture", this));
    }
    public void stopCapture()
    {
        if (GetComponent<MotionSession>().PlaybackVisible) { ReportWarning("回放中不会发送设备命令。"); return; }
        CancelPreparation();
        var gpsController = GetComponent<GPSController>();
        if (null != gpsController)
        {
            gpsController.StopUpdateGPS();
        }
        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandStopCapture, ref pendingCommand);
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "Stop Capture", this));
    }
    public void handleStartRecord() {
        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandStartRecored, ref pendingCommand);
        var takeName = PlayerPrefs.GetString("Record.TakeName", "");
        if (takeName != "") {
            var ptr = Marshal.StringToHGlobalAnsi(takeName);
            IMCPCommand.Command.SetCommandExtraLong((uint)EMCPCommandExtraLong.CommandExtraLong_Extra1, ptr, pendingCommand);
        }
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "Start Record", this));
    }
    public void handleStopRecord() {
        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandStopRecored, ref pendingCommand);
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "Stop Record", this));
    }
    public void handleReset0MotionDrift() { PrepareAction("重置零漂", "请按设备要求保持静止。", handleReset0MotionDriftNow); }
    void handleReset0MotionDriftNow()
    {
        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandClearZeroMotionDrift, ref pendingCommand);
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "Reset 0 Motion Drift", this));
    }
    public void handleControlButton()
    {
        ControlPanel.SetActive(true);
    }
    public void handleMotionButton()
    {
        MotionPanel.SetActive(true);
    }
    public void handleZeroPosition() { PrepareAction("位置归零", "请站到要设为原点的位置。", handleZeroPositionNow); }
    void handleZeroPositionNow()
    {
        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandZeroPosition, ref pendingCommand);
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "Zero Position", this));
    }
    public void handleCalibration()
    {
        if (IsCalibrating) { calibrationGuide.Show(calibrationProcess.Guide); return; }
        PrepareAction("开始校准", "请走到测试位置，随后按设备提示完成 A / T / F-Pose。", BeginCalibration);
    }
    void BeginCalibration()
    {
        if (calibrationGuide == null) calibrationGuide = CalibrationGuidePanel.Create(transform);
        if (calibrationProcess != null && !calibrationProcess.isComplete())
        {
            calibrationGuide.Show(calibrationProcess.Guide);
            return;
        }
        var guide = new CalibrationGuideState();
        calibrationGuide.Show(guide);
        ulong pendingCommand = 0;
        var error = IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandCalibrateMotion, ref pendingCommand);
        if (error != EMCPError.Error_None || pendingCommand == 0)
        {
            guide.Finish(false, "无法开始校准，请检查设备连接后重试。");
            ReportError($"Create calibration command failed: {error}.");
            return;
        }
        calibrationProcess = new CalibrationProcess(pendingCommand, this, guide);
        if (!GetComponent<MocapApplication>().queueCommand(pendingCommand, calibrationProcess))
            guide.Finish(false, "校准请求未能发送，请检查设备连接后重试。");
    }
    public void handleResumeBody() { PrepareAction("恢复身体姿态", "请摆好要恢复的身体姿态。", handleResumeBodyNow); }
    void handleResumeBodyNow()
    {
        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandResumeOriginalPosture, ref pendingCommand);
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "Resume Body", this));
    }
    public void handleResumeHands() { PrepareAction("恢复手部姿态", "请摆好要恢复的手部姿态。", handleResumeHandsNow); }
    void handleResumeHandsNow()
    {
        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandResumeOriginalHandsPosture, ref pendingCommand);
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "Resume Hands", this));
    }
    public void handlePilotSimulationDirvingOff()
    {
        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandPilotSimulationDriving, ref pendingCommand);
        IMCPCommand.Command.SetCommandExtraLong((uint)EMCPCommandExtraLong.CommandExtraLong_Extra1, new IntPtr(0), pendingCommand);
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "PilotSimulationDirving-Off", this));
    }
    public void handlePilotSimulationDirvingOn()
    {

        ulong pendingCommand = 0;
        IMCPCommand.Command.CreateCommand((uint)EMCPCommand.CommandPilotSimulationDriving, ref pendingCommand);
        IMCPCommand.Command.SetCommandExtraLong((uint)EMCPCommandExtraLong.CommandExtraLong_Extra1, new IntPtr(1), pendingCommand);
        GetComponent<MocapApplication>().queueCommand(pendingCommand, new NormalCommandProcess(pendingCommand, "PilotSimulationDirving-on", this));
    }

    public void handleSettings()
    {
        if(Workspace != null && Workspace.IsReady) Workspace.ShowTab(2); else pendingSettings=true;
    }
    public void handleInformation() { if(Workspace!=null)Workspace.ShowTab(2); }
    public void handleDevice() { if(Workspace!=null)Workspace.ShowSensors(); }


    StatusPanel statusPanel
    {
        get
        {
            if (null != _statusPanel)
            {
                return _statusPanel;
            }
            _statusPanel = transform.Find("Canvas").Find("StatusPanel").GetComponent<StatusPanel>();
            return _statusPanel;
        }
    }
    OutputPanel outputPanel
    {
        get
        {
            if (null != _outputPanel)
            {
                return _outputPanel;
            }
            _outputPanel = transform.Find("Canvas").Find("OutputPanel").GetComponent<OutputPanel>();
            return _outputPanel;
        }
    }

    public void handleOutput() { if(Workspace!=null)Workspace.ShowTab(3); }
    public void handleGPSTracker()
    {
        if (null == GPSTrackerPanel)
        {
            return;
        }
        if (GPSTrackerPanel.activeSelf)
        {
            GPSTrackerPanel.SetActive(false);
        }
        else
        {
            GPSTrackerPanel.SetActive(true);
        }
    }

    public void ReportError(string msg)
    {

        if (Workspace != null) Workspace.ShowMessage(msg, true);
        statusPanel.ReportError(msg);
        outputPanel.ReportError(msg);
    }
    public void ReportWarning(string msg)
    {
        if (Workspace != null) Workspace.ShowMessage(msg, true);
        statusPanel.ReportWarning(msg);
        outputPanel.ReportWarning(msg);
    }
    public void ReportInfo(string msg)
    {
        if (Workspace != null) Workspace.ShowMessage(msg, false);
        statusPanel.ReportInfo(msg);
        outputPanel.ReportInfo(msg);
    }
}
