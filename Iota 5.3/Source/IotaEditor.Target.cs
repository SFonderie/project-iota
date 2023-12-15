// Copyright Sydney Fonderie. All Rights Reserved.

using UnrealBuildTool;

public class IotaEditorTarget : TargetRules
{
	public IotaEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		bUseUnityBuild = false;

		ExtraModuleNames.Add("IotaGame");
	}
}
