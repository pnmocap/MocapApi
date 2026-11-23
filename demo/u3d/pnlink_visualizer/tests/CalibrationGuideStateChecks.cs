using System;

public static class CalibrationGuideStateChecks
{
    static int checks;
    static void Check(bool valid, string message)
    {
        ++checks;
        if (!valid) throw new Exception(message);
    }
    public static int Main()
    {
        var state = new CalibrationGuideState();
        Check(state.IsRunning && state.PoseName == "" && state.CurrentPoseIndex == -1,
            "Waiting must not invent the first pose");
        string[] deviceOrder = { "F-Pose", "T-Pose", "Custom device pose" };
        state.SetSupportedPoses(deviceOrder);
        deviceOrder[0] = "changed externally";
        Check(state.SupportedPoses[0] == "F-Pose", "Device order must be copied without a fixed A/T/F sequence");
        state.Report(CalibrationGuidePhase.Preparing, "Custom device pose");
        Check(state.CurrentPoseIndex == 2 && state.PoseName == "Custom device pose", "Unknown pose names must remain usable");
        state.Report(CalibrationGuidePhase.Countdown, "T-Pose", 4);
        Check(state.Countdown == 4 && state.CurrentPoseIndex == 1, "Countdown belongs to the reported pose");
        state.Acknowledge();
        Check(state.Phase == CalibrationGuidePhase.Countdown && state.Countdown == 4, "Late response must not reset pose progress");
        state.ReportReadIssue();
        Check(state.PoseName == "T-Pose" && state.IsRunning, "Read failure must not advance or complete calibration");
        state.Report(CalibrationGuidePhase.Capturing, "T-Pose", 200);
        Check(state.Progress == 100 && state.Countdown == 0 && state.IsRunning, "100 percent is not final command success");
        state.Finish(true, "done");
        state.Report(CalibrationGuidePhase.Preparing, "F-Pose");
        state.Finish(false, "late failure");
        Check(state.Phase == CalibrationGuidePhase.Succeeded && state.PoseName == "T-Pose", "Terminal result must not be changed by late callbacks");
        var failed = new CalibrationGuideState();
        failed.Finish(false, "device rejection");
        Check(!failed.IsRunning && failed.Message == "device rejection", "Device failure must stay visible");
        var interrupted = new CalibrationGuideState();
        interrupted.Interrupt("connection reset");
        interrupted.Finish(true, "late result");
        Check(interrupted.Phase == CalibrationGuidePhase.Interrupted && !interrupted.IsRunning, "Connection reset must not appear successful");
        var empty = new CalibrationGuideState();
        empty.SetSupportedPoses(new[] { "", "  ", " Z Pose " });
        empty.Report(CalibrationGuidePhase.Preparing, "Z Pose");
        Check(empty.SupportedPoses.Count == 1 && empty.CurrentPoseIndex == 0, "Empty pose labels should not corrupt step numbering");
        Console.WriteLine("PASS " + checks + " calibration state checks");
        return 0;
    }
}
