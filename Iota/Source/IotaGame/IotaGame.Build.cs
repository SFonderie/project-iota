// Copyright Sydney Fonderie, 2023. All Rights Reserved.

using UnrealBuildTool;

public class IotaGame : ModuleRules
{
	public IotaGame(ReadOnlyTargetRules Target) : base(Target)
	{
		// Enable Include-What-You-Use in the module.
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// CORE ENGINE MODULES
		PublicDependencyModuleNames.Add("Core");
		PublicDependencyModuleNames.Add("CoreUObject");
		PublicDependencyModuleNames.Add("Engine");

		// GAMEPLAY MODULES
		PublicDependencyModuleNames.Add("GameplayAbilities");
		PublicDependencyModuleNames.Add("GameplayTags");
		PublicDependencyModuleNames.Add("GameplayTasks");

		// IOTA MODULES
		PublicDependencyModuleNames.Add("IotaCore");
		PublicDependencyModuleNames.Add("IotaTile");
	}
}
