// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;

public class ue4_mocap : ModuleRules
{
    public ue4_mocap(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;


        //PublicIncludePaths.Add();
        var MocapApiPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../../../"));
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        PublicIncludePaths.Add(Path.Combine(MocapApiPath, "include"));
        PublicAdditionalLibraries.Add(Path.Combine(MocapApiPath, "lib/win32/x64", "MocapApi.lib"));
        var MocapApiDllPath = Path.Combine(MocapApiPath, "bin/win32/x64", "MocapApi.dll");
        PublicDelayLoadDLLs.Add(MocapApiDllPath);
        RuntimeDependencies.Add(MocapApiDllPath, StagedFileType.NonUFS);
    }
}
