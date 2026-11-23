using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using MocapApi;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

// Included only in the isolated validation player, never in the delivery DLL.
public sealed class CalibrationGuideRuntimeProbe : MonoBehaviour
{
    string output;
    int checks;
    bool done;
    sealed class FakeCommand : AbstractCommandProcess
    {
        public string Order = "";
        public FakeCommand() : base(123456, "probe") { }
        public override void handleResponse() { Order += "response;"; }
        public override void handleRunning() { Order += "running;"; }
        public override void handleResult() { Order += "result;"; }
        public override bool isComplete() { return false; }
    }

    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.AfterSceneLoad)]
    static void Install()
    {
        foreach (var app in UnityEngine.Object.FindObjectsByType<MocapApplication>(FindObjectsInactive.Include, FindObjectsSortMode.None))
            app.enabled = false; // Before Start: no native application/socket or device command.
        var go = new GameObject("CalibrationGuideRuntimeProbe");
        go.AddComponent<CalibrationGuideRuntimeProbe>();
    }
    void Check(bool success, string message)
    {
        if (!success) throw new Exception("PROBE: " + message);
        ++checks;
    }
    void Awake()
    {
        Application.runInBackground = true;
        var args = Environment.GetCommandLineArgs();
        output = args[Array.IndexOf(args, "--guide-probe-output") + 1];
        Directory.CreateDirectory(output);
        Application.logMessageReceived += OnLog;
    }
    void OnLog(string message, string trace, LogType type)
    {
        if (!done && (type == LogType.Exception || type == LogType.Assert || type == LogType.Error))
        {
            done = true;
            File.WriteAllText(Path.Combine(output, "failure.txt"), message + "\n" + trace);
            Application.Quit(7);
        }
    }
    IEnumerator Screenshot(string name)
    {
        yield return null;
        yield return new WaitForEndOfFrame();
        var texture = ScreenCapture.CaptureScreenshotAsTexture();
        File.WriteAllBytes(Path.Combine(output, name + ".png"), texture.EncodeToPNG());
        Destroy(texture);
    }
    IEnumerator Start()
    {
        yield return null;
        var app = UnityEngine.Object.FindFirstObjectByType<MocapApplication>(FindObjectsInactive.Include);
        Check(app != null && !app.enabled, "native connection disabled");
        var field = typeof(MocapApplication).GetField("pendingCommandMap", BindingFlags.Instance | BindingFlags.NonPublic);
        var commands = (Dictionary<ulong, AbstractCommandProcess>)field.GetValue(app);
        var fake = new FakeCommand();
        commands.Add(fake.command, fake);
        var events = new MCPEvent_t[3];
        EMCPReplay[] replies = { EMCPReplay.MCPReplay_Response, EMCPReplay.MCPReplay_Running, EMCPReplay.MCPReplay_Result };
        for (int i = 0; i < events.Length; ++i)
        {
            events[i].eventType = EMCPEventType.MCPEvent_CommandReply;
            events[i].eventData.commandRespond._commandHandle = fake.command;
            events[i].eventData.commandRespond._replay = replies[i];
        }
        app.DispatchEvents(events, 3);
        Check(fake.Order == "response;running;result;", "all three events dispatched in order");
        Check(commands.Count == 0, "completed command removed");

        var main = app.GetComponent<MainPanel>();
        main.handleCalibration(); // Local CreateCommand only; disabled app has handle zero.
        var created = (CalibrationProcess)typeof(MainPanel).GetField("calibrationProcess", BindingFlags.Instance | BindingFlags.NonPublic).GetValue(main);
        var initialPanel = (CalibrationGuidePanel)typeof(MainPanel).GetField("calibrationGuide", BindingFlags.Instance | BindingFlags.NonPublic).GetValue(main);
        Check(initialPanel.gameObject.activeInHierarchy, "Calibration button immediately opens a popup");
        Check(created != null && created.isComplete() && created.Guide.Phase == CalibrationGuidePhase.Failed,
            "disconnected calibration request is released and shown as failure");
        Check(commands.Count == 0, "failed queue does not leave a pending command");
        initialPanel.Hide();

        var model = new CalibrationGuideState();
        model.SetSupportedPoses(new[] { "A-Pose", "T-Pose", "F-Pose" });
        var panel = CalibrationGuidePanel.Create(app.transform);
        panel.Show(model);
        Check(panel.gameObject.activeInHierarchy, "popup opens immediately while waiting");
        yield return Screenshot("01-waiting");
        model.Report(CalibrationGuidePhase.Preparing, "T-Pose");
        yield return Screenshot("02-prepare");
        var pose = panel.transform.Find("ModalShade/GuideCard/Pose").GetComponent<Text>();
        Check(pose.text == "T-Pose" && pose.fontSize >= 80, "large current pose label");
        Check(pose.font != null && pose.font.HasCharacter('校'), "Chinese guide font exists");
        var results = new List<RaycastResult>();
        EventSystem.current.RaycastAll(new PointerEventData(EventSystem.current) { position = new Vector2(5, 5) }, results);
        Check(results.Count > 0 && results[0].gameObject.name == "ModalShade", "modal prevents click-through");
        model.Report(CalibrationGuidePhase.Countdown, "T-Pose", 3);
        yield return Screenshot("03-countdown");
        Check(panel.transform.Find("ModalShade/GuideCard/Value").GetComponent<Text>().text == "3 秒", "device countdown displayed");
        model.Report(CalibrationGuidePhase.Capturing, "T-Pose", 64);
        yield return Screenshot("04-capture");
        var fill = (RectTransform)panel.transform.Find("ModalShade/GuideCard/ProgressTrack/ProgressFill");
        Check(Mathf.Abs(fill.anchorMax.x - .64f) < .001f, "progress bar matches device percentage");
        panel.Hide();
        Check(model.IsRunning, "hiding does not finish/cancel calibration");
        typeof(MainPanel).GetField("calibrationProcess", BindingFlags.Instance | BindingFlags.NonPublic).SetValue(main, new CalibrationProcess(987654, main, model));
        typeof(MainPanel).GetField("calibrationGuide", BindingFlags.Instance | BindingFlags.NonPublic).SetValue(main, panel);
        main.handleCalibration();
        Check(panel.gameObject.activeInHierarchy && model.Progress == 64, "repeated click only reopens existing calibration");
        model.Finish(true, "校准完成，可以开始动作测试。");
        yield return Screenshot("05-success");
        Check(pose.text == "校准完成", "success remains visible");
        var failure = new CalibrationGuideState();
        panel.Show(failure);
        failure.Finish(false, "设备拒绝校准，请先开始采集后重试。");
        yield return Screenshot("06-failure");
        Check(pose.text == "校准失败", "failure remains visible");
        done = true;
        File.WriteAllText(Path.Combine(output, "passed.json"), "{\"passed\":true,\"checks\":" + checks + ",\"device_commands_sent\":0,\"screenshots\":6}");
        Application.Quit(0);
    }
}
