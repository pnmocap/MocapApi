using MocapApi;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

public class CalibrationProcess : AbstractCommandProcess
{
    readonly AbstractReporter reporter;
    ulong progressHandle;
    string lastReadError;
    public CalibrationGuideState Guide { get; private set; }

    public CalibrationProcess(ulong command, AbstractReporter reporter) :
        this(command, reporter, new CalibrationGuideState()) { }
    public CalibrationProcess(ulong command, AbstractReporter reporter, CalibrationGuideState guide) :
        base(command, "Calibration")
    {
        this.reporter = reporter;
        Guide = guide;
    }
    public override void handleResponse() { Guide.Acknowledge(); }
    public override void handleResult()
    {
        if (command == 0) return;
        try
        {
            uint result = 0;
            var error = IMCPCommand.Command.GetCommandResultCode(ref result, command);
            if (error != EMCPError.Error_None)
            {
                Guide.Finish(false, "无法确认校准结果，请检查设备连接后重试。");
                reporter.ReportError($"Read calibration result failed: {error}.");
            }
            else if (result != 0)
            {
                string message = null;
                IMCPCommand.Command.GetCommandResultMessage(ref message, command);
                Guide.Finish(false, string.IsNullOrWhiteSpace(message) ?
                    $"校准失败（{result}），请检查设备后重试。" : "校准失败：" + message);
                reporter.ReportError($"Calibration error ({result}): {message}");
            }
            else
            {
                Guide.Finish(true, "校准完成，可以开始动作测试。");
                reporter.ReportInfo("Calibration done.");
            }
        }
        finally { DestroyCommand(); }
    }
    void ReadSupportedPoses()
    {
        uint count = 0;
        var api = IMCPCalibrateMotionProgress.CalibrateMotionProgress;
        var error = api.GetCalibrateMotionProgressCountOfSupportPoses(ref count, progressHandle);
        if (error != EMCPError.Error_None || count > 64) return;
        var names = new List<string>();
        for (uint i = 0; i < count; ++i)
        {
            string name;
            error = api.GetCalibrateMotionProgressNameOfSupportPose(out name, i, progressHandle);
            if (error != EMCPError.Error_None) return;
            names.Add(name);
        }
        Guide.SetSupportedPoses(names);
    }
    void ReadIssue(string operation, EMCPError error)
    {
        Guide.ReportReadIssue();
        string message = $"Calibration {operation}: {error}.";
        if (message != lastReadError) reporter.ReportWarning(message);
        lastReadError = message;
    }
    public override void handleRunning()
    {
        if (command == 0 || !Guide.IsRunning) return;
        if (progressHandle == 0)
        {
            IntPtr buffer = Marshal.AllocHGlobal(sizeof(ulong));
            EMCPError error;
            try
            {
                Marshal.WriteInt64(buffer, 0);
                error = IMCPCommand.Command.GetCommandProgress(
                    (uint)EMCPCommandProgress.CommandProgress_CalibrateMotion, buffer, command);
                if (error == EMCPError.Error_None) progressHandle = (ulong)Marshal.ReadInt64(buffer);
            }
            finally { Marshal.FreeHGlobal(buffer); }
            if (error != EMCPError.Error_None || progressHandle == 0)
            {
                ReadIssue("read progress handle", error);
                return;
            }
            ReadSupportedPoses();
        }
        uint step = 0;
        string pose;
        var api = IMCPCalibrateMotionProgress.CalibrateMotionProgress;
        var readError = api.GetCalibrateMotionProgressStepOfCurrentPose(ref step, out pose, progressHandle);
        if (readError != EMCPError.Error_None) { ReadIssue("read current pose", readError); return; }
        if (step == (uint)EMCPCalibrateMotionProgressStep.CalibrateMotionProgressStep_Prepare)
            Guide.Report(CalibrationGuidePhase.Preparing, pose);
        else if (step == (uint)EMCPCalibrateMotionProgressStep.CalibrateMotionProgressStep_Countdown)
        {
            uint countdown = 0;
            readError = api.GetCalibrateMotionProgressCountdownOfCurrentPose(ref countdown, out pose, progressHandle);
            if (readError != EMCPError.Error_None) { ReadIssue("read countdown", readError); return; }
            Guide.Report(CalibrationGuidePhase.Countdown, pose, countdown);
        }
        else if (step == (uint)EMCPCalibrateMotionProgressStep.CalibrateMotionProgressStep_Progress)
        {
            uint progress = 0;
            readError = api.GetCalibrateMotionProgressProgressOfCurrentPose(ref progress, out pose, progressHandle);
            if (readError != EMCPError.Error_None) { ReadIssue("read capture progress", readError); return; }
            Guide.Report(CalibrationGuidePhase.Capturing, pose, progress);
        }
        else { ReadIssue("unknown progress step " + step, EMCPError.Error_NotSupported); return; }
        lastReadError = null;
    }
    public override void handleInterrupted(string reason)
    {
        Guide.Interrupt(reason);
        DestroyCommand();
    }
    public override bool isComplete() { return command == 0; }
}
