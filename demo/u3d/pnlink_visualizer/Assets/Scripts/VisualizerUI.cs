using System;
using UnityEngine;
using UnityEngine.UI;

// Shared runtime UI uses the fonts already shipped with the matching Unity player.
public static class VisualizerUI
{
    public static readonly Color Ink = new Color(.90f, .94f, 1f);
    public static readonly Color Muted = new Color(.59f, .67f, .78f);
    public static readonly Color Blue = new Color(.16f, .48f, .98f);
    static Font font;
    static Sprite circle;
    public static Image Dot(string name, Transform parent, Color color, float l, float b, float r, float t)
    {
        if(circle==null) {
            var texture=new Texture2D(32,32,TextureFormat.RGBA32,false);
            var pixels=new Color[32*32];
            for(int y=0;y<32;y++)for(int x=0;x<32;x++) {
                float radius=new Vector2(x-15.5f,y-15.5f).magnitude;
                pixels[y*32+x]=new Color(1,1,1,Mathf.Clamp01(16-radius));
            }
            texture.SetPixels(pixels);texture.Apply();circle=Sprite.Create(texture,new Rect(0,0,32,32),new Vector2(.5f,.5f));
        }
        var dot=Box(name,parent,color,l,b,r,t);dot.sprite=circle;dot.preserveAspect=true;return dot;
    }
    public static Font Font { get {
        if (font == null) font = Resources.Load<Font>("OPPOSans-Regular");
        if (font == null) font = UnityEngine.Font.CreateDynamicFontFromOSFont(new[] { "Microsoft YaHei", "Arial" }, 24);
        return font;
    } }
    public static RectTransform Rect(string name, Transform parent, float l, float b, float r, float t)
    {
        var rect = new GameObject(name, typeof(RectTransform)).GetComponent<RectTransform>();
        rect.SetParent(parent, false); rect.anchorMin = new Vector2(l, b); rect.anchorMax = new Vector2(r, t);
        rect.offsetMin = rect.offsetMax = Vector2.zero; return rect;
    }
    public static Image Box(string name, Transform parent, Color color, float l, float b, float r, float t, bool block = false)
    {
        var image = Rect(name, parent, l, b, r, t).gameObject.AddComponent<Image>();
        image.color = color; image.raycastTarget = block; return image;
    }
    public static Text Label(string name, Transform parent, string value, int size, Color color,
        float l, float b, float r, float t, TextAnchor align = TextAnchor.MiddleLeft)
    {
        var text = Rect(name, parent, l, b, r, t).gameObject.AddComponent<Text>();
        text.font = Font; text.fontSize = size; text.color = color; text.text = value; text.alignment = align;
        text.raycastTarget = false; text.supportRichText = false;
        text.resizeTextForBestFit = true; text.resizeTextMinSize = Math.Min(size, 12); text.resizeTextMaxSize = size;
        text.gameObject.AddComponent<LocalizedText>();
        return text;
    }
    public static Button Button(string name, Transform parent, string value, Action action,
        float l, float b, float r, float t, bool primary = false)
    {
        var image = Box(name, parent, primary ? Blue : new Color(.16f,.21f,.30f), l,b,r,t,true);
        var button = image.gameObject.AddComponent<Button>(); button.targetGraphic = image;
        button.onClick.AddListener(() => action());
        Label("Caption", image.transform, value, 19, Ink, .04f,0,.96f,1,TextAnchor.MiddleCenter);
        return button;
    }
    public static Canvas Canvas(string name, Transform parent, int order)
    {
        var go = new GameObject(name, typeof(RectTransform),typeof(Canvas),typeof(CanvasScaler),typeof(GraphicRaycaster));
        go.transform.SetParent(parent,false);
        var c = go.GetComponent<Canvas>(); c.renderMode = RenderMode.ScreenSpaceOverlay; c.sortingOrder = order;
        var scaler = go.GetComponent<CanvasScaler>(); scaler.uiScaleMode = CanvasScaler.ScaleMode.ScaleWithScreenSize;
        scaler.referenceResolution = new Vector2(1280,800); scaler.matchWidthOrHeight = .5f;
        return c;
    }
}

public static class VisualizerFeedback
{
    static AudioClip tick, ready;
    public static void Play(GameObject owner, bool completed)
    {
        if (tick == null) tick = Tone(620, .08f);
        if (ready == null) ready = Tone(980, .35f);
        var audio = owner.GetComponent<AudioSource>();
        if (audio == null) audio = owner.AddComponent<AudioSource>();
        audio.spatialBlend = 0; audio.playOnAwake = false; audio.PlayOneShot(completed ? ready : tick, .4f);
    }
    static AudioClip Tone(float hz, float seconds)
    {
        int count = (int)(22050 * seconds); var values = new float[count];
        for (int i=0;i<count;i++) values[i] = Mathf.Sin(2*Mathf.PI*hz*i/22050) * Mathf.Sin(Mathf.PI*i/count) * .3f;
        var clip = AudioClip.Create("Visualizer cue",count,1,22050,false); clip.SetData(values,0); return clip;
    }
}

public sealed class SoloActionPrompt : MonoBehaviour
{
    Action pending;
    double due;
    int lastSeconds;
    Text number, title, hint;
    public bool IsPending { get { return pending != null; } }
    public static SoloActionPrompt Create(Transform owner)
    {
        var canvas = VisualizerUI.Canvas("SoloActionPrompt", owner, 31000);
        var prompt = canvas.gameObject.AddComponent<SoloActionPrompt>();
        var shade = VisualizerUI.Box("Shade", canvas.transform,new Color(.02f,.03f,.06f,.87f),0,0,1,1,true);
        var card = VisualizerUI.Box("Card",shade.transform,new Color(.08f,.12f,.19f),.16f,.16f,.84f,.84f,true);
        prompt.title = VisualizerUI.Label("Title",card.transform,"",34,VisualizerUI.Ink,.05f,.76f,.95f,.94f,TextAnchor.MiddleCenter);
        prompt.number = VisualizerUI.Label("Seconds",card.transform,"5",150,VisualizerUI.Ink,.1f,.30f,.9f,.77f,TextAnchor.MiddleCenter);
        prompt.hint = VisualizerUI.Label("Hint",card.transform,"",24,VisualizerUI.Muted,.06f,.18f,.94f,.32f,TextAnchor.MiddleCenter);
        VisualizerUI.Button("Cancel",card.transform,"取消本次操作",prompt.Cancel,.30f,.04f,.70f,.15f);
        canvas.gameObject.SetActive(false); return prompt;
    }
    public bool Begin(string operation, string instruction, Action action)
    {
        if (IsPending) return false;
        title.text = operation + " · 5 秒后执行"; hint.text = instruction;
        pending = action; due = Time.realtimeSinceStartupAsDouble + 5; lastSeconds = 5;
        number.text = "5"; gameObject.SetActive(true); VisualizerFeedback.Play(gameObject,false); return true;
    }
    public void Cancel() { pending = null; gameObject.SetActive(false); }
    void OnDisable() { pending = null; }
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape)) { Cancel(); return; }
        if (pending == null) return;
        double remaining = due - Time.realtimeSinceStartupAsDouble;
        if (remaining > 0) {
            int seconds = (int)Math.Ceiling(remaining);
            if (seconds != lastSeconds) { lastSeconds = seconds; number.text = seconds.ToString(); VisualizerFeedback.Play(gameObject,false); }
            return;
        }
        var action = pending; pending = null;
        VisualizerFeedback.Play(gameObject,true); gameObject.SetActive(false); action();
    }
}
