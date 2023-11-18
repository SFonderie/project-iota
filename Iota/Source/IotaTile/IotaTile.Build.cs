// Copyright Sydney Fonderie, 2023. All Rights Reserved.

using UnrealBuildTool;

public class IotaTile : ModuleRules
{
	public IotaTile(ReadOnlyTargetRules Target) : base(Target)
	{
		// Enable Include-What-You-Use in the module.
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// CORE ENGINE MODULES
		PublicDependencyModuleNames.Add("Core");
		PublicDependencyModuleNames.Add("CoreUObject");
		PublicDependencyModuleNames.Add("Engine");

		// SPECIFIC MODULES
		PublicDependencyModuleNames.Add("GameplayTags");
		PublicDependencyModuleNames.Add("NavigationSystem");

		// IOTA MODULES
		PublicDependencyModuleNames.Add("IotaCore");
	}
}
