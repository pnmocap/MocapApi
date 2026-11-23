# Changelog

## 0.8.1 — 2026-11-23

- Remove the legacy Android plugin, manifest, mobile scenes and location code from the
  Windows demo. Keep the pinned Windows SDK pair.

## 0.8.0 — 2026-11-23

First versioned publication of the PN Link Visualizer Unity demo, based on the validated
internal r8 implementation. Product version, Unity bundleVersion, UI, build manifest
and release tag now use a single `version.json` definition.

- Chinese/English interface with separate live and replay avatars/cameras.
- Recent-motion connection indicator, current and recorded device/sensor status.
- Cancellable five-second preparation for calibration and posture operations.
- F-Pose walking, final standing-still cue and processing/free-movement feedback.
- Local skeletal recording/replay, pause/seek/speed and NVPOSE1/NVPOSE2 compatibility.
- Readable connection settings and sensor details; safe invalid-IP/occupied-port startup.
- Existing minimal Unity demo, repository SDK and Nclib algorithms are unchanged.
- Pin the tested MocapApi 0.0.70.49068d72 DLL/binding pair; keep optional GPS fixtures local.

Historical r6/r7/r8 packages retain their original names and fingerprints.
