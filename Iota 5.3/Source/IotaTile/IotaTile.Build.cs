// Copyright Sydney Fonderie. All Rights Reserved.

using UnrealBuildTool;

public class IotaTile : ModuleRules
{
	public IotaTile(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"AssetRegistry",
				"GameplayTags",
			}
		);
	}
}
