using UnrealBuildTool;

public class IotaTarget : TargetRules
{
	public IotaTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		// IWYU Enforcement
		bUseUnityBuild = false;
		bUsePCHFiles = false;

		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;

		ExtraModuleNames.Add("IotaCore");
	}
}
