using UnityEngine;
using UnityEngine.UI;

// Created at runtime: existing scenes and serialized component links stay intact.
public sealed class CalibrationGuidePanel : MonoBehaviour
{
    readonly Color ink = new Color(0.09f, 0.14f, 0.23f);
    readonly Color muted = new Color(0.37f, 0.43f, 0.52f);
    readonly Color blue = new Color(0.09f, 0.42f, 0.94f);
    CalibrationGuideState state;
    Font font;
    Text stepLabel, poseLabel, phaseLabel, valueLabel, instruction, sequence, hint, closeLabel;
    Image accent, progressFill;
    RectTransform progressTrack;
    int renderedRevision = -1;
    bool renderedStale;
    bool lastStanding;
    int languageRevision=-1;
    CalibrationGuidePhase lastPhase = CalibrationGuidePhase.Waiting;

    public static CalibrationGuidePanel Create(Transform owner)
    {
        var root = new GameObject("CalibrationGuide", typeof(RectTransform), typeof(Canvas),
            typeof(CanvasScaler), typeof(GraphicRaycaster));
        root.transform.SetParent(owner, false);
        var canvas = root.GetComponent<Canvas>();
        canvas.renderMode = RenderMode.ScreenSpaceOverlay;
        canvas.overrideSorting = true;
        canvas.sortingOrder = 30000;
        var scaler = root.GetComponent<CanvasScaler>();
        scaler.uiScaleMode = CanvasScaler.ScaleMode.ScaleWithScreenSize;
        scaler.referenceResolution = new Vector2(1280, 800);
        scaler.matchWidthOrHeight = 0.5f;
        var panel = root.AddComponent<CalibrationGuidePanel>();
        panel.Build();
        root.SetActive(false);
        return panel;
    }

    public void Show(CalibrationGuideState guide)
    {
        state = guide;
        renderedRevision = -1;
        gameObject.SetActive(true);
        transform.SetAsLastSibling();
        Refresh();
    }

    public void Hide()
    {
        // Hiding the overlay is deliberately not a device cancellation command.
        gameObject.SetActive(false);
    }

    void Update()
    {
        if (state == null) return;
        bool stale = state.IsRunning && state.SecondsSinceLastReport >= 10;
        if (renderedRevision != state.Revision || stale != renderedStale || languageRevision!=VLoc.Revision) Refresh();
    }

    void Refresh()
    {
        if (state == null) return;
        languageRevision=VLoc.Revision;
        if(state.IsStandingFinish && !lastStanding)VisualizerFeedback.Play(gameObject,true);
        lastStanding=state.IsStandingFinish;
        renderedRevision = state.Revision;
        renderedStale = state.IsRunning && state.SecondsSinceLastReport >= 10;
        bool success = state.Phase == CalibrationGuidePhase.Succeeded;
        bool processing = state.Phase == CalibrationGuidePhase.Processing;
        if (lastPhase != state.Phase && (processing || success)) VisualizerFeedback.Play(gameObject, true);
        lastPhase = state.Phase;
        Color color = success ? new Color(0.08f, 0.57f, 0.35f) :
            state.IsRunning ? blue : new Color(0.78f, 0.19f, 0.21f);
        accent.color = color;
        progressFill.color = color;
        valueLabel.color = color;
        poseLabel.color = state.IsRunning ? ink : color;
        int index = state.CurrentPoseIndex;
        stepLabel.text = !state.IsRunning ? "校准结果" : index >= 0 ?
            $"当前姿势 · 第 {index + 1} / {state.SupportedPoses.Count} 步" : "当前姿势 · 以设备提示为准";
        poseLabel.text = processing ? "可以自由活动" : success ? "校准完成" : state.Phase == CalibrationGuidePhase.Interrupted ?
            "连接中断" : state.Phase == CalibrationGuidePhase.Failed ? "校准失败" :
            string.IsNullOrEmpty(state.PoseName) ? "等待设备…" : state.PoseName;
        switch (state.Phase)
        {
            case CalibrationGuidePhase.Preparing: phaseLabel.text = "请准备这个姿势"; valueLabel.text = "准备"; break;
            case CalibrationGuidePhase.Countdown: phaseLabel.text = state.IsWalkingPose ? "准备走路 · 设备倒计时" : "保持姿势 · 设备倒计时"; valueLabel.text = state.Countdown + " 秒"; break;
            case CalibrationGuidePhase.Capturing: phaseLabel.text = state.IsStandingFinish?"停止走路 · 站稳不动":state.IsWalkingPose ? "自然走路 · 正在采集" : "保持姿势 · 正在采集"; valueLabel.text = state.Progress + "%"; break;
            case CalibrationGuidePhase.Processing: phaseLabel.text = "采集结束 · 正在处理校准数据"; valueLabel.text = "处理中"; break;
            case CalibrationGuidePhase.Succeeded: phaseLabel.text = "设备已确认完成"; valueLabel.text = "完成"; break;
            case CalibrationGuidePhase.Failed: phaseLabel.text = "本次校准未成功"; valueLabel.text = "未完成"; break;
            case CalibrationGuidePhase.Interrupted: phaseLabel.text = "无法继续确认设备进度"; valueLabel.text = "已中断"; break;
            default: phaseLabel.text = "正在等待设备响应"; valueLabel.text = "等待"; break;
        }
        instruction.text = state.Message;
        progressTrack.gameObject.SetActive(state.Phase == CalibrationGuidePhase.Capturing || success || processing);
        var fill = progressFill.rectTransform;
        fill.anchorMax = new Vector2(success || processing ? 1 : state.Progress / 100f, 1);
        fill.offsetMin = fill.offsetMax = Vector2.zero;
        var names = state.SupportedPoses;
        string path = "";
        for (int i = 0; i < names.Count; ++i)
            path += (i == 0 ? "" : "  →  ") + (i == index && state.IsRunning ? "【" + names[i] + "】" : names[i]);
        sequence.text = names.Count == 0 ? "校准顺序由设备返回，收到提示后再摆姿势。" : path;
        hint.text = processing ? "已响提示音；无需继续行走。处理完成后会再次提示，尚未代表校准成功。" :
            renderedStale ? "暂未收到新的设备进度，请检查连接；当前姿势不会自动跳过。" :
            state.IsRunning ? "提示会自动切换。收起窗口不会取消校准，可再次点击 Calibration 查看。" :
            success ? "关闭窗口后即可查看动作效果。" : "请确认设备状态后重新开始校准。";
        closeLabel.text = state.IsRunning ? "收起引导" : "关闭";
    }

    void Build()
    {
        font = Resources.Load<Font>("OPPOSans-Regular");
        if (font == null) font = Resources.Load<Font>("hanyiqiuyiheiultraw");
        if (font == null) font = Font.CreateDynamicFontFromOSFont(new[] { "Microsoft YaHei", "Arial" }, 28);
        var shade = Box("ModalShade", transform, new Color(0.02f, 0.04f, 0.09f, 0.78f), 0, 0, 1, 1);
        shade.raycastTarget = true;
        var card = Box("GuideCard", shade.transform, new Color(0.98f, 0.985f, 1f), .07f, .075f, .93f, .925f);
        card.raycastTarget = true;
        accent = Box("Accent", card.transform, blue, 0, .985f, 1, 1);
        Label("Heading", card.transform, "校准引导", 28, ink, TextAnchor.MiddleLeft, .06f, .875f, .65f, .96f);
        Label("HeadingEnglish", card.transform, "CALIBRATION", 15, muted, TextAnchor.MiddleRight, .65f, .875f, .94f, .96f);
        Box("HeaderRule", card.transform, new Color(.86f, .89f, .94f), .06f, .867f, .94f, .869f);
        stepLabel = Label("Step", card.transform, "", 22, muted, TextAnchor.MiddleCenter, .06f, .77f, .94f, .84f);
        poseLabel = Label("Pose", card.transform, "", 96, ink, TextAnchor.MiddleCenter, .06f, .555f, .94f, .785f);
        poseLabel.resizeTextForBestFit = true;
        poseLabel.resizeTextMinSize = 38;
        poseLabel.resizeTextMaxSize = 96;
        phaseLabel = Label("Phase", card.transform, "", 25, muted, TextAnchor.MiddleCenter, .06f, .485f, .94f, .55f);
        valueLabel = Label("Value", card.transform, "", 64, blue, TextAnchor.MiddleCenter, .06f, .355f, .94f, .485f);
        var track = Box("ProgressTrack", card.transform, new Color(.87f, .91f, .97f), .2f, .322f, .8f, .336f);
        progressTrack = track.rectTransform;
        progressFill = Box("ProgressFill", track.transform, blue, 0, 0, 0, 1);
        instruction = Label("Instruction", card.transform, "", 27, ink, TextAnchor.MiddleCenter, .07f, .22f, .93f, .31f);
        instruction.resizeTextForBestFit = true;
        instruction.resizeTextMinSize = 18;
        instruction.resizeTextMaxSize = 27;
        sequence = Label("Sequence", card.transform, "", 18, muted, TextAnchor.MiddleCenter, .06f, .15f, .94f, .22f);
        sequence.resizeTextForBestFit = true;
        sequence.resizeTextMinSize = 12;
        sequence.resizeTextMaxSize = 18;
        hint = Label("Hint", card.transform, "", 16, muted, TextAnchor.MiddleLeft, .06f, .025f, .75f, .13f);
        var buttonImage = Box("Close", card.transform, new Color(.89f, .93f, .99f), .77f, .045f, .94f, .125f);
        buttonImage.raycastTarget = true;
        var button = buttonImage.gameObject.AddComponent<Button>();
        button.targetGraphic = buttonImage;
        button.onClick.AddListener(Hide);
        closeLabel = Label("Caption", button.transform, "", 21, blue, TextAnchor.MiddleCenter, 0, 0, 1, 1);
    }

    static RectTransform Rect(string name, Transform parent, float left, float bottom, float right, float top)
    {
        var go = new GameObject(name, typeof(RectTransform));
        var rect = go.GetComponent<RectTransform>();
        rect.SetParent(parent, false);
        rect.anchorMin = new Vector2(left, bottom);
        rect.anchorMax = new Vector2(right, top);
        rect.offsetMin = rect.offsetMax = Vector2.zero;
        return rect;
    }
    static Image Box(string name, Transform parent, Color color, float left, float bottom, float right, float top)
    {
        var image = Rect(name, parent, left, bottom, right, top).gameObject.AddComponent<Image>();
        image.color = color;
        image.raycastTarget = false;
        return image;
    }
    Text Label(string name, Transform parent, string text, int size, Color color, TextAnchor alignment,
        float left, float bottom, float right, float top)
    {
        var label = Rect(name, parent, left, bottom, right, top).gameObject.AddComponent<Text>();
        label.font = font;
        label.fontSize = size;
        label.color = color;
        label.alignment = alignment;
        label.supportRichText = false;
        label.raycastTarget = false;
        label.text = text;
        label.gameObject.AddComponent<LocalizedText>();
        return label;
    }
}
