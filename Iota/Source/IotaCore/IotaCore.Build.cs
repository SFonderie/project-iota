// Copyright Sydney Fonderie, 2023. All Rights Reserved.

using UnrealBuildTool;

public class IotaCore : ModuleRules
{
	public IotaCore(ReadOnlyTargetRules Target) : base(Target)
	{
		// Enable Include-What-You-Use in the module.
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// CORE ENGINE MODULES
		PublicDependencyModuleNames.Add("Core");
		PublicDependencyModuleNames.Add("CoreUObject");
		PublicDependencyModuleNames.Add("Engine");

		// GAMEPLAY MODULES
		PrivateDependencyModuleNames.Add("GameplayAbilities");
		PrivateDependencyModuleNames.Add("GameplayTags");
		PrivateDependencyModuleNames.Add("GameplayTasks");
	}
}
