using UnrealBuildTool;
using System.Collections.Generic;

public class IotaEditorTarget : TargetRules
{
	public IotaEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Iota" } );
	}
}
