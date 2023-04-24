using UnrealBuildTool;

public class IotaEditorTarget : TargetRules
{
	public IotaEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		// IWYU Enforcement
		bUseUnityBuild = false;
		bUsePCHFiles = false;

		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;

		ExtraModuleNames.Add("IotaCore");
	}
}
