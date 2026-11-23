# SDK dependency

This demo pins **MocapApi 0.0.70.49068d72** for its tested Windows x64 player:

- C# binding: `Assets/Scripts/MocapApi.cs`
- Native DLL: `Assets/Plugins/MocapApi.dll`
- SHA-256 fingerprints: `version.json` → `sdk`

The runtime shows the loaded SDK's own version separately from the product version.
The repository root SDK may be newer than this demo's validated pair. Do not replace
one side independently. Validate a dependency update before changing the pinned pair.

The repository's existing `install.bat` still serves the original SDK demos and is not
changed by this addition. This demo already includes its paired Windows dependency.
Historical Android assets are retained without claiming Android player validation.
