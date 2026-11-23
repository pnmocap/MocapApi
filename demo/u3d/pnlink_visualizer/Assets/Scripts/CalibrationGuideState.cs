using System;
using System.Collections.Generic;
using System.Diagnostics;

public enum CalibrationGuidePhase
{
    Waiting, Preparing, Countdown, Capturing, Succeeded, Failed, Interrupted, Processing
}

// Device reports determine pose order and timing; no local timer advances poses.
public sealed class CalibrationGuideState
{
    string[] poses = new string[0];
    IReadOnlyList<string> readonlyPoses = Array.AsReadOnly(new string[0]);
    public CalibrationGuidePhase Phase { get; private set; } = CalibrationGuidePhase.Waiting;
    public string PoseName { get; private set; } = "";
    public string Message { get; private set; } = "正在请求校准，请等待设备返回当前姿势。";
    public uint Countdown { get; private set; }
    public uint Progress { get; private set; }
    static string PoseKey(string name) { return (name ?? "").Replace("-", "").Replace("_", "").Replace(" ", ""); }
    public bool IsWalkingPose { get { return PoseKey(PoseName).Equals("FPose", StringComparison.OrdinalIgnoreCase); } }
    public bool IsStandingFinish { get { return IsWalkingPose && Phase==CalibrationGuidePhase.Capturing && Progress>=80; } }
    public int Revision { get; private set; }
    public long LastReportTimestamp { get; private set; } = Stopwatch.GetTimestamp();
    public IReadOnlyList<string> SupportedPoses { get { return readonlyPoses; } }
    public bool IsRunning { get { return Phase != CalibrationGuidePhase.Succeeded &&
        Phase != CalibrationGuidePhase.Failed && Phase != CalibrationGuidePhase.Interrupted; } }
    public int CurrentPoseIndex
    {
        get
        {
            for (int i = 0; i < poses.Length; ++i)
                if (string.Equals(PoseKey(poses[i]), PoseKey(PoseName), StringComparison.OrdinalIgnoreCase)) return i;
            return -1;
        }
    }
    public double SecondsSinceLastReport
    {
        get { return (Stopwatch.GetTimestamp() - LastReportTimestamp) / (double)Stopwatch.Frequency; }
    }
    public void SetSupportedPoses(IEnumerable<string> supported)
    {
        if (!IsRunning || supported == null) return;
        var names = new List<string>();
        foreach (string name in supported)
        {
            if (!string.IsNullOrWhiteSpace(name)) names.Add(name.Trim());
            if (names.Count == 64) break;
        }
        poses = names.ToArray();
        readonlyPoses = Array.AsReadOnly(poses);
        Touch();
    }
    public void Acknowledge()
    {
        if (!IsRunning) return;
        if (Phase == CalibrationGuidePhase.Waiting) Message = "设备已响应，正在等待当前姿势。";
        Touch();
    }
    public void Report(CalibrationGuidePhase phase, string poseName, uint value = 0)
    {
        if (!IsRunning) return;
        if (phase != CalibrationGuidePhase.Preparing && phase != CalibrationGuidePhase.Countdown &&
            phase != CalibrationGuidePhase.Capturing) throw new ArgumentOutOfRangeException(nameof(phase));
        if (Phase == CalibrationGuidePhase.Processing) return; // Delayed capture reports cannot undo collection completion.
        Phase = phase;
        PoseName = (poseName ?? "").Trim();
        Countdown = phase == CalibrationGuidePhase.Countdown ? value : 0;
        Progress = phase == CalibrationGuidePhase.Capturing ? Math.Min(value, 100u) : 0;
        if (IsWalkingPose && phase == CalibrationGuidePhase.Capturing && Progress == 100 &&
            (SupportedPoses.Count == 0 || CurrentPoseIndex == SupportedPoses.Count - 1))
        {
            Phase = CalibrationGuidePhase.Processing;
            Message = "采集已结束，可以自由活动。正在处理校准数据，请等待结果。";
        }
        else if(IsStandingFinish)
            Message="请停止走路，双脚站稳，双臂自然下垂，保持不动直到采集结束。";
        else if (IsWalkingPose)
            Message = phase == CalibrationGuidePhase.Capturing ? "请自然向前走路，双臂自然摆动；末段听到提示后停下站稳。" :
                "F-Pose 是行走校准。请留出安全空间，倒计时结束后自然走路。";
        else Message = phase == CalibrationGuidePhase.Preparing ? "请摆好当前姿势，保持身体稳定。" :
            phase == CalibrationGuidePhase.Countdown ? "请保持当前姿势，等待倒计时结束。" :
            "正在采集，请保持当前姿势不动。";
        Touch();
    }
    public void ReportReadIssue()
    {
        if (!IsRunning) return;
        Message = Phase == CalibrationGuidePhase.Processing ? "采集已结束，可以自由活动；暂时无法读取处理进度，请等待设备结果。" :
            IsStandingFinish ? "暂时无法读取进度；请站稳不动，等待设备确认采集结束。" :
            IsWalkingPose ? "暂时无法读取进度，请检查连接，并按当前行走提示继续。" :
            "暂时无法读取设备校准进度，请检查连接并保持当前姿势。";
        Touch();
    }
    public void Finish(bool succeeded, string message)
    {
        if (!IsRunning) return;
        Phase = succeeded ? CalibrationGuidePhase.Succeeded : CalibrationGuidePhase.Failed;
        Message = string.IsNullOrWhiteSpace(message) ?
            (succeeded ? "校准完成，可以开始动作测试。" : "校准失败，请检查设备后重试。") : message;
        Touch();
    }
    public void Interrupt(string message)
    {
        if (!IsRunning) return;
        Phase = CalibrationGuidePhase.Interrupted;
        Message = message;
        Touch();
    }
    void Touch() { LastReportTimestamp = Stopwatch.GetTimestamp(); ++Revision; }
}
