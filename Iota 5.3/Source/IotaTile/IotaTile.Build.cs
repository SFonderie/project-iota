// Copyright Sydney Fonderie. All Rights Reserved.

using UnrealBuildTool;

public class IotaTile : ModuleRules
{
	public IotaTile(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.Add("Core");
		PublicDependencyModuleNames.Add("CoreUObject");
		PublicDependencyModuleNames.Add("Engine");
		PublicDependencyModuleNames.Add("GameplayTags");
	}
}
