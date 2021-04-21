// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ue4_mocap : ModuleRules
{
	public ue4_mocap(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		PublicIncludePaths.Add("E:\\work\\MocapApi\\include");
		PublicAdditionalLibraries.Add("E:\\work\\MocapApi\\vs141_dbg\\lib\\Debug\\MocapApiD.lib");
		PublicDelayLoadDLLs.Add("MocapApiD.dll");
		RuntimeDependencies.Add("E:\\work\\MocapApi\\vs141_dbg\\bin\\Debug\\MocapApiD.dll", StagedFileType.NonUFS);
		RuntimeDependencies.Add("E:\\work\\MocapApi\\vs141_dbg\\bin\\Debug\\MocapApiD.pdb", StagedFileType.DebugNonUFS);
	}
}
