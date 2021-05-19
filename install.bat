echo %~dp0bin\win32\x64\MocapApi.dll


set UE4_BINARIES_PATH=%~dp0demo\ue4\ue4_mocap\Binaries\Win64
if not exist %UE4_BINARIES_PATH% (mkdir %UE4_BINARIES_PATH%)
copy %~dp0bin\win32\x64\MocapApi.dll %UE4_BINARIES_PATH%


echo %~dp0include\MocapApi.cs

copy %~dp0include\MocapApi.cs demo\u3d\u3d_mocap\Assets

copy %~dp0bin\win32\x64\MocapApi.dll demo\u3d\u3d_mocap\Assets