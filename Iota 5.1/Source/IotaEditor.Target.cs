// Copyright Sydney Fonderie, 2023. All Rights Reserved.

using UnrealBuildTool;

public class IotaEditorTarget : TargetRules
{
	public IotaEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		// Enforce Include-What-You-Use
		bUseUnityBuild = false;
		bUsePCHFiles = false;

		// Follow default standards for UE5 Projects.
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.Add("IotaGame");
	}
}
