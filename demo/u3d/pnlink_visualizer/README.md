# PN Link Visualizer 0.8.0

A Unity demo for receiving motion data through MocapApi, guiding device calibration,
and recording/replaying the received skeletal results. This demo is independent from
`../u3d_mocap`, which remains the minimal SDK example.

**Status:** preview. Automated Windows checks have passed; live, worn-device calibration
and motion quality still need device testing. The historical internal revision is r8.

## Open and build

1. Open this directory with **Unity 6000.0.31f1**.
2. Let Unity restore packages listed in `Packages/manifest.json`.
3. Open `Assets/Scenes/visualizer.unity` and build **Windows x86_64**.
4. Keep the bundled MocapApi DLL and C# binding together; see [SDK-DEPENDENCIES.md](SDK-DEPENDENCIES.md).

The SDK dependency is **0.0.70.49068d72**. It is separate from the Visualizer product version.
Android legacy assets are retained, but Android/Linux player builds are not validated
by this release. The main build scene is the Windows visualizer scene.

## Use

- **Connection:** set the PC receive IPv4/UDP port and the device command IPv4/port.
  Defaults are receive port **8002** and device command port **8080**. The PC address
  must belong to an active network interface and match the device's motion destination.
- **Live test:** start capture, calibrate, restore body/hand posture, zero position,
  and start/stop local recording. A green dot indicates recent received motion.
- Actions requiring preparation have a cancellable **5-second countdown**.
- **F-Pose:** walk naturally, then stop and stand still at the final cue. The UI asks
  you to stop at 80% reported progress. Remain still until collection reaches 100%.
  Only then may you move freely while waiting for the final calibration result.
- **Record & replay:** open a `.mocap` take; play/pause, seek and change playback speed.
  Live and replay use separate avatars, cameras and status views.
- The top-right language control switches between Chinese and English.

Recordings are streamed to `Application.persistentDataPath/Recordings`. Use the folder
button to find them. The legacy Unity persistence identity `Noitom/visualizer` is kept
to preserve existing preferences and recordings; the visible product/window name is
**PN Link Visualizer**. Normal application exit finalizes an active recording.

`.mocap` is this demo's versioned binary format. It stores received display poses and
local timestamps; NVPOSE2 also includes historical device/sensor snapshots. It does not
contain raw IMU data. See [MOCAP-FORMAT.txt](MOCAP-FORMAT.txt).

## Version management

`version.json` is the source of truth for product version, build number, tag, Unity
version, SDK fingerprint and recording compatibility. The release tag is
**pnlink-visualizer-v0.8.0**. SDK and device/algorithm versions are independent.

After editing `version.json`, run:

```text
python tools/sync-version.py --write
python tools/sync-version.py
```

The generated `Assets/Scripts/VisualizerBuildInfo.cs` and Unity `bundleVersion` must be
committed together. [CHANGELOG.md](CHANGELOG.md) records user-visible changes.

## Validation and script-only builds

`tests/PNLinkStateChecks.cs` covers recording/status rules, and
`tests/PNLinkRuntimeProbe.cs` covers the private offline Unity workflow probe. Test
sources are outside `Assets` and never included in a normal player.

The Windows helper `tools/build-player-scripts.py` recompiles against an existing
matching Unity player. Supply `--compiler` or `CSC_EXE` for Roslyn. It is intended for
script changes and a guarded equal-width bundleVersion update; it is not a replacement
for Unity Editor when scenes/resources or serialized layouts change.

Example:

```text
python tools/build-player-scripts.py --base-player <matching-player> --output <new-output> --compiler <csc.exe>
```

Do not add serializable fields to scene-bound legacy components when using this
helper: existing binary scenes retain their original serialized layout.

The optional historical GPS sample reads a local `test_data/gps_reocrd_0.txt`. Personal
fixtures and recordings are not distributed; the sample disables itself when absent.

## 中文说明

本工程是增强版 Unity Demo，保留原 `u3d_mocap` 示例。正式工具版本为 **0.8.0**，
构建号 **1**，对应标签 `pnlink-visualizer-v0.8.0`；历史内部迭代号为 r8。

使用 Unity **6000.0.31f1** 打开本目录，构建 `Assets/Scenes/visualizer.unity` 的
Windows x64 运行包。SDK 版本单独固定为 **0.0.70.49068d72**，不要只替换 DLL 或绑定文件之一。

实时页完成采集、校准和录制；回放页查看已录动作及录制时的设备状态。需要摆姿势的操作
有可取消的 5 秒倒计时。F-Pose 先自然行走，末段按提示停下站稳，直到采集结束后才能自由活动。
右上角可切换中英文。录制文件是本 Demo 自定义的骨骼结果格式，不是原始 IMU 录制。

本版通过离线自动化与 Windows 启动检查，穿戴设备的校准效果仍需要实际测试。
