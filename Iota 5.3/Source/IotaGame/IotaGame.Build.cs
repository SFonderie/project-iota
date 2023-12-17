// Copyright Sydney Fonderie. All Rights Reserved.

using UnrealBuildTool;

public class IotaGame : ModuleRules
{
	public IotaGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"IotaTile",
			}
		);
	}
}
