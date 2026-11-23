using System;
using System.Net;
using System.Net.Sockets;

// Check in managed code before handing endpoints to the native SDK. The SDK in
// r5 can terminate the process when asked to bind an unavailable local address.
public static class MocapConnectionSettings
{
    public static bool Validate(string localIP, int localPort, string remoteIP, int remotePort, out string error)
    {
        error = null;
        IPAddress local, remote;
        if (!IPAddress.TryParse(localIP, out local) || local.AddressFamily != AddressFamily.InterNetwork)
            error = "本地 IP 必须是有效的 IPv4 地址。";
        else if (!IPAddress.TryParse(remoteIP, out remote) || remote.AddressFamily != AddressFamily.InterNetwork ||
                 remote.Equals(IPAddress.Any) || remote.Equals(IPAddress.Broadcast))
            error = "设备 IP 必须是有效的 IPv4 单播地址。";
        else if (localPort < 1 || localPort > 65535 || remotePort < 1 || remotePort > 65535)
            error = "UDP 端口必须在 1 到 65535 之间。";
        return error == null;
    }

    public static bool CanBind(string localIP, int localPort, out string error)
    {
        error = null;
        try
        {
            using (var socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp))
            {
                socket.ExclusiveAddressUse = true;
                socket.Bind(new IPEndPoint(IPAddress.Parse(localIP), localPort));
            }
            return true;
        }
        catch (SocketException e)
        {
            if (e.SocketErrorCode == SocketError.AddressNotAvailable)
                error = $"本地 IP {localIP} 当前不在电脑上。请连接设备网卡，或在 Settings 中修改本地 IP。";
            else if (e.SocketErrorCode == SocketError.AddressAlreadyInUse || e.SocketErrorCode == SocketError.AccessDenied)
                error = $"本地 UDP {localIP}:{localPort} 无法使用。请关闭其他 Visualizer，或在 Settings 中修改端口。";
            else
                error = $"无法使用本地 UDP {localIP}:{localPort}：{e.SocketErrorCode}。请检查 Settings。";
        }
        catch (Exception e) when (e is ArgumentException || e is FormatException)
        {
            error = "本地 IP 或端口无效，请在 Settings 中修改。";
        }
        return false;
    }
}
