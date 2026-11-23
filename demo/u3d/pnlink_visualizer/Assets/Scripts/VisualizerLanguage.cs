using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using UnityEngine;
using UnityEngine.UI;

public static class VLoc
{
    public static bool English { get; private set; }
    public static int Revision { get; private set; }
    public static void Load() { English=PlayerPrefs.GetInt("PNLink.Language",0)==1; ++Revision; }
    public static void Toggle() { English=!English;PlayerPrefs.SetInt("PNLink.Language",English?1:0);PlayerPrefs.Save();++Revision; }
    public static string Pick(string zh,string en) { return English?en:zh; }
    static readonly Dictionary<string,string> words=new Dictionary<string,string> {
        {"实时测试","Live test"},{"录制回放","Record & replay"},{"设备连接","Connection"},{"更多工具","Tools"},
        {"准备","Get ready"},{"等待","Waiting"},{"完成","Done"},{"处理中","Processing"},{"未完成","Not completed"},{"已中断","Interrupted"},
        {"播放","Play"},{"暂停","Pause"},{"关闭","Close"},{"收起引导","Hide guide"},{"取消本次操作","Cancel this action"},
        {"校准引导","Calibration guide"},{"当前姿势 · 以设备提示为准","Current pose · follow the device"},{"校准结果","Calibration result"},
        {"请准备这个姿势","Prepare for this pose"},{"保持姿势 · 设备倒计时","Hold pose · device countdown"},
        {"准备走路 · 设备倒计时","Get ready to walk · device countdown"},{"自然走路 · 正在采集","Walk naturally · collecting"},
        {"保持姿势 · 正在采集","Hold pose · collecting"},{"停止走路 · 站稳不动","Stop walking · stand still"},
        {"采集结束 · 正在处理校准数据","Collection ended · processing calibration"},{"可以自由活动","You may move freely"},
        {"设备已确认完成","Device confirmed completion"},{"本次校准未成功","Calibration did not succeed"},{"无法继续确认设备进度","Device progress is unavailable"},
        {"正在等待设备响应","Waiting for the device"},{"等待设备…","Waiting for device…"},{"校准完成","Calibration complete"},{"校准失败","Calibration failed"},{"连接中断","Connection interrupted"},
        {"正在请求校准，请等待设备返回当前姿势。","Requesting calibration. Wait for the device pose prompt."},
        {"设备已响应，正在等待当前姿势。","Device acknowledged. Waiting for the current pose."},
        {"请摆好当前姿势，保持身体稳定。","Take the indicated pose and keep your body steady."},
        {"请保持当前姿势，等待倒计时结束。","Hold this pose until the countdown ends."},
        {"正在采集，请保持当前姿势不动。","Collecting data. Keep this pose still."},
        {"F-Pose 是行走校准。请留出安全空间，倒计时结束后自然走路。","F-Pose is a walking calibration. Make room, then walk after the countdown."},
        {"请自然向前走路，双臂自然摆动；末段听到提示后停下站稳。","Walk naturally with relaxed arm swing. Stop and stand still at the final cue."},
        {"请停止走路，双脚站稳，双臂自然下垂，保持不动直到采集结束。","Stop walking. Stand firmly with arms relaxed. Stay still until collection ends."},
        {"采集已结束，可以自由活动。正在处理校准数据，请等待结果。","Collection has ended. You may move freely while calibration is processed."},
        {"校准完成，可以开始动作测试。","Calibration complete. You can start testing motion."},
        {"校准失败，请检查设备后重试。","Calibration failed. Check the device and retry."},
        {"暂时无法读取设备校准进度，请检查连接并保持当前姿势。","Cannot read calibration progress. Check the connection and hold the current pose."},
        {"暂时无法读取进度，请检查连接，并按当前行走提示继续。","Progress is unavailable. Check the connection and follow the current walking cue."},
        {"采集已结束，可以自由活动；暂时无法读取处理进度，请等待设备结果。","Collection ended. You may move freely; wait for the device result."},
        {"暂时无法读取进度；请站稳不动，等待设备确认采集结束。","Progress is unavailable. Stay still until the device confirms collection has ended."},
        {"采集结束，可以自由活动。正在处理校准数据。","Collection ended. You may move freely while calibration is processed."},
        {"已响提示音；无需继续行走。处理完成后会再次提示，尚未代表校准成功。","Collection ended. You may move freely. A final result is still required."},
        {"暂未收到新的设备进度，请检查连接；当前姿势不会自动跳过。","No new progress received. Check the connection; poses are not skipped automatically."},
        {"提示会自动切换。收起窗口不会取消校准，可再次点击 Calibration 查看。","Follow the changing prompts. Hiding the guide does not cancel calibration."},
        {"关闭窗口后即可查看动作效果。","Close the guide to inspect the motion."},{"请确认设备状态后重新开始校准。","Check the device before starting calibration again."},
        {"校准顺序由设备返回，收到提示后再摆姿势。","The device supplies the pose order. Wait for its prompt."},
        {"开始校准","Start calibration"},{"恢复身体姿态","Restore body pose"},{"恢复手部姿态","Restore hand pose"},{"位置归零","Zero position"},
        {"重置零漂","Reset zero drift"},{"开始采集","Start capture"},{"开始采集（PTP）","Start capture (PTP)"},{"开始本机录制","Start local recording"},
        {"请走到测试位置，倒计时结束后开始采集。","Move into position. Capture starts after the countdown."},{"请走到测试位置。","Move into position."},
        {"请摆好要恢复的身体姿态。","Take the body pose you want to restore."},{"请摆好要恢复的手部姿态。","Take the hand pose you want to restore."},
        {"请站到要设为原点的位置。","Stand at the position you want to use as the origin."},{"请按设备要求保持静止。","Keep still as required by the device."},
        {"请走到测试位置，随后按设备提示完成 A / T / F-Pose。","Move into position, then follow the A / T / F-Pose prompts."},
        {"请准备动作，倒计时结束后开始记录。","Prepare your movement. Recording starts after the countdown."},
        {"请先返回实时，再发送设备操作。","Switch to Live test before operating the device."},{"校准正在进行，请先等待结果。","Calibration is running. Wait for its result."},
        {"设备正在执行其他操作，请稍后重试。","Another device command is running. Please wait."},
        {"设备连接尚未准备好，命令未发送。","The device connection is not ready. The command was not sent."},
        {"无法开始校准，请检查设备连接后重试。","Cannot start calibration. Check the connection and retry."},
        {"校准请求未能发送，请检查设备连接后重试。","Calibration request could not be sent. Check the connection and retry."},
        {"无法确认校准结果，请检查设备连接后重试。","Cannot verify the calibration result. Check the connection and retry."},
        {"连接已关闭，无法继续确认本次校准状态。","Connection closed. Calibration status can no longer be verified."},
        {"回放中不会发送设备命令。","Device commands are disabled in replay."},{"回放中不发送设备命令。","Device commands are disabled in replay."},
        {"尚未收到实时动作，请先开始采集。","No live motion received. Start capture first."},{"本机录制已开始。","Local recording started."},
        {"没有收到动作帧，未生成空录像。","No motion frames were received. No empty take was saved."},
        {"正在读取录像，请稍候。","Reading a recording. Please wait."},{"请先结束录制再回放。","Stop recording before opening a take."},
        {"请先结束当前设备操作，再进入回放。","Finish the current device action before loading a recording."},
        {"正在读取并校验录像，可随时点击返回实时取消。","Reading and validating the recording. Cancel loading to return to live."},
        {"已恢复完整录像帧。尾部未完成的数据不会播放。","Complete frames recovered. The incomplete trailing frame will be skipped."},
        {"录像已载入，点击播放。","Recording loaded. Press Play."},{"已取消载入录像。","Recording load cancelled."},
        {"已返回实时，等待新的动作数据。","Returned to live. Waiting for motion."},
        {"单段录制达到容量上限，已保存。可开始下一段录制。","Recording limit reached. The take was saved; you can start another."},
        {"本地 IP 必须是有效的 IPv4 地址。","Local IP must be a valid IPv4 address."},{"设备 IP 必须是有效的 IPv4 单播地址。","Device IP must be a valid unicast IPv4 address."},
        {"UDP 端口必须在 1 到 65535 之间。","UDP ports must be between 1 and 65535."},{"UDP 端口必须是 1 到 65535 的整数。","UDP ports must be integers between 1 and 65535."},
        {"本地 IP 或端口无效，请在 Settings 中修改。","Invalid local IP or port. Correct it in Connection."},
        {"设备配置已保存。","Connection settings saved."},
        {"骨骼数量无效。","Invalid bone count."},{"骨骼路径过长。","Bone path is too long."},{"动作帧时间或骨骼数量无效。","Invalid frame time or bone count."},
        {"动作数据包含非有限数值。","Motion contains non-finite values."},{"不是 Visualizer 动作录制文件。","Not a supported PN Link recording."},
        {"骨骼路径无效。","Invalid bone path."},{"骨骼路径重复。","Duplicate bone path."},{"没有完整帧或录制过长。","No complete frames, or the recording is too long."},
        {"录制过长。","Recording exceeds the supported length."},{"录制时间轴无效。","Invalid recording timeline."},{"位置数据无效。","Invalid position data."},{"旋转数据无效。","Invalid rotation data."},
        {"旋转四元数无效。","Invalid rotation quaternion."},{"录像骨骼与当前模型不一致。","Recording bones do not match this model."},
        {"实时 · 等待动作数据","Live · waiting for motion"},{"实时 · 正在接收动作","Live · receiving motion"},
        {"校准处理中 · 可自由活动","Processing calibration · free to move"},{"正在校准 · 按大窗口提示操作","Calibrating · follow the guide"},
        {"正在载入录像…","Loading recording…"}
    };
    public static string T(string text)
    {
        if(!English||string.IsNullOrEmpty(text))return text;
        string value;if(words.TryGetValue(text,out value))return value;
        var m=Regex.Match(text,@"^当前姿势 · 第 (\d+) / (\d+) 步$");
        if(m.Success)return "Current pose · step "+m.Groups[1]+" / "+m.Groups[2];
        m=Regex.Match(text,@"^(\d+) 秒$");if(m.Success)return m.Groups[1]+" s";
        if(text.EndsWith(" · 5 秒后执行",StringComparison.Ordinal))return T(text.Substring(0,text.Length-" · 5 秒后执行".Length))+" · starts in 5 seconds";
        string[,] prefixes={ {"已保存：","Saved: "},{"无法开始录制：","Cannot start recording: "},{"录制已停止：","Recording stopped: "},
            {"录像结束异常，已写入数据保留为 .partial：","Recording interrupted; written data remains in .partial: "},
            {"无法打开录像：","Cannot open recording: "},{"回放停止：","Replay stopped: "},{"无法读取录像目录：","Cannot read recording folder: "},
            {"无法打开目录：","Cannot open folder: "},{"设备连接初始化失败：","Device initialization failed: "},{"校准失败：","Calibration failed: "} };
        for(int i=0;i<prefixes.GetLength(0);i++)if(text.StartsWith(prefixes[i,0],StringComparison.Ordinal))return prefixes[i,1]+T(text.Substring(prefixes[i,0].Length));
        if(text.StartsWith("本地 IP ",StringComparison.Ordinal))return "Local IP is not assigned to this PC. Connect the device network or correct the local IP in Connection.";
        if(text.StartsWith("本地 UDP ",StringComparison.Ordinal))return "Local UDP endpoint is unavailable. Close other Visualizer instances or change the port in Connection.";
        if(text.StartsWith("无法使用本地 UDP ",StringComparison.Ordinal))return "Cannot bind the local UDP endpoint. Check Connection settings.";
        return text; // Preserve native device details, paths, identifiers and technical messages.
    }
}

public sealed class BilingualLabel : MonoBehaviour
{
    public string Chinese,English;
    int revision=-1;
    void LateUpdate() { if(revision==VLoc.Revision)return; revision=VLoc.Revision;GetComponent<Text>().text=VLoc.Pick(Chinese,English); }
}

public sealed class LocalizedText : MonoBehaviour
{
    Text label;
    string source,rendered;
    void LateUpdate()
    {
        if(label==null)label=GetComponent<Text>();
        if(label.text!=rendered)source=label.text;
        rendered=VLoc.T(source);label.text=rendered;
    }
}
