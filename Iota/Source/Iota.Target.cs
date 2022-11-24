using UnrealBuildTool;
using System.Collections.Generic;

public class IotaTarget : TargetRules
{
	public IotaTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Iota" } );
	}
}
