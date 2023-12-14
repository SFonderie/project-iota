// Copyright Sydney Fonderie, 2024. All Rights Reserved.

using UnrealBuildTool;

public class IotaGame : ModuleRules
{
	public IotaGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.Add("Core");
		PublicDependencyModuleNames.Add("CoreUObject");
		PublicDependencyModuleNames.Add("Engine");
		PublicDependencyModuleNames.Add("InputCore");
	}
}
