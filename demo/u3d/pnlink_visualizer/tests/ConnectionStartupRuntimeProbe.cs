using System;
using System.Collections;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using UnityEngine;

// Private test build only. Unlike the guide-only probe, leaves native Start enabled.
public sealed class ConnectionStartupRuntimeProbe : MonoBehaviour
{
    string output;
    int checks;
    bool done;
    string savedLocal, savedRemote;
    int savedLocalPort, savedRemotePort;
    MocapApplication app;
    MainPanel main;
    Settings settings;

    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.AfterSceneLoad)]
    static void Install() { new GameObject("ConnectionStartupRuntimeProbe").AddComponent<ConnectionStartupRuntimeProbe>(); }
    void Check(bool value, string message)
    {
        if (!value) throw new Exception("CONNECTION PROBE: " + message);
        ++checks;
        Debug.Log("CONNECTION CHECK: " + message);
    }
    ulong Handle => (ulong)typeof(MocapApplication).GetField("applicationHandle", BindingFlags.Instance | BindingFlags.NonPublic).GetValue(app);
    bool Open => (bool)typeof(MocapApplication).GetField("connectionOpen", BindingFlags.Instance | BindingFlags.NonPublic).GetValue(app);
    void Awake()
    {
        Application.runInBackground = true;
        var args = Environment.GetCommandLineArgs();
        output = args[Array.IndexOf(args, "--guide-probe-output") + 1];
        Directory.CreateDirectory(output);
        savedLocal = PlayerPrefs.GetString("UDP.Local.IP", "127.0.0.1");
        savedRemote = PlayerPrefs.GetString("UDP.Remote.IP", "127.0.0.1");
        savedLocalPort = PlayerPrefs.GetInt("UDP.Local.Port", 8002);
        savedRemotePort = PlayerPrefs.GetInt("UDP.Remote.Port", 8080);
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
    void SetEndpoints(string local, string port)
    {
        main.handleSettings();
        settings.UDPLoacalIP.text = local;
        settings.UDPLoacalPort.text = port;
        settings.UDPRemoteIP.text = "127.0.0.1";
        settings.UDPRemotePort.text = "58081";
        settings.okButtonHandle();
    }
    IEnumerator Start()
    {
        yield return new WaitForSecondsRealtime(2);
        app = UnityEngine.Object.FindFirstObjectByType<MocapApplication>();
        main = app.GetComponent<MainPanel>();
        settings = main.SettingsPanel.GetComponent<Settings>();
        Check(app.enabled && Handle == 0 && !Open, "unavailable saved local IP is rejected before native initialization");
        Check(main.SettingsPanel.activeInHierarchy, "settings automatically opens on startup failure");
        yield return new WaitForEndOfFrame();
        var texture = ScreenCapture.CaptureScreenshotAsTexture();
        File.WriteAllBytes(Path.Combine(output, "startup-unavailable-ip.png"), texture.EncodeToPNG());
        Destroy(texture);
        SetEndpoints("127.0.0.1", "not-a-port");
        Check(main.SettingsPanel.activeInHierarchy && PlayerPrefs.GetInt("UDP.Local.Port") == savedLocalPort,
            "non-numeric port stays editable and does not overwrite saved port");
        SetEndpoints("127.0.0.1", "65536");
        Check(main.SettingsPanel.activeInHierarchy && Handle == 0, "out-of-range port does not initialize native SDK");
        SetEndpoints("invalid-address", "58082");
        Check(main.SettingsPanel.activeInHierarchy && Handle == 0, "invalid IPv4 stays editable");
        SetEndpoints("127.0.0.1", "58082");
        yield return new WaitForSecondsRealtime(3);
        Check(Open && Handle != 0 && !main.SettingsPanel.activeInHierarchy, "corrected settings initialize real native SDK without restarting");
        string error;
        Check(!MocapConnectionSettings.CanBind("127.0.0.1", 58082, out error), "native SDK really owns the UDP receive port");
        app.reset();
        yield return new WaitForSecondsRealtime(1);
        Check(Open && Handle != 0, "reapplying same settings releases and reopens the native port");
        using (var busy = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp))
        {
            busy.ExclusiveAddressUse = true;
            busy.Bind(new IPEndPoint(IPAddress.Loopback, 58083));
            SetEndpoints("127.0.0.1", "58083");
            yield return null;
            Check(!Open && Handle == 0 && main.SettingsPanel.activeInHierarchy, "occupied port is rejected before SDK open");
        }
        SetEndpoints("127.0.0.1", "58083");
        yield return new WaitForSecondsRealtime(1);
        Check(Open && Handle != 0, "retry succeeds after occupied port is released");
        SetEndpoints(savedLocal, savedLocalPort.ToString());
        yield return null;
        Check(!Open && Handle == 0 && main.SettingsPanel.activeInHierarchy, "switching back to unavailable IP is recoverable");
        done = true;
        File.WriteAllText(Path.Combine(output, "passed.json"), "{\"passed\":true,\"checks\":" + checks + ",\"native_connection_enabled\":true,\"device_commands_sent\":0}");
        RestorePrefs();
        Application.Quit(0);
    }
    void RestorePrefs()
    {
        PlayerPrefs.SetString("UDP.Local.IP", savedLocal);
        PlayerPrefs.SetString("UDP.Remote.IP", savedRemote);
        PlayerPrefs.SetInt("UDP.Local.Port", savedLocalPort);
        PlayerPrefs.SetInt("UDP.Remote.Port", savedRemotePort);
        PlayerPrefs.Save();
    }
    void OnDestroy() { RestorePrefs(); }
}
