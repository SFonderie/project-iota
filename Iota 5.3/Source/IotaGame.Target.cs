// Copyright Sydney Fonderie. All Rights Reserved.

using UnrealBuildTool;

public class IotaGameTarget : TargetRules
{
	public IotaGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		bUseUnityBuild = false;

		ExtraModuleNames.Add("IotaGame");
	}
}
