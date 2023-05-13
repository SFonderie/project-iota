// Copyright Sydney Fonderie, 2023. All Rights Reserved.

using UnrealBuildTool;

public class IotaGameTarget : TargetRules
{
	public IotaGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		// Include-What-You-Use
		bUseUnityBuild = false;
		bUsePCHFiles = false;

		// Follow default standards for UE5 Projects.
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.Add("IotaCore");
	}
}
