using UnrealBuildTool;

public class Composite : ModuleRules
{
	public Composite(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
	}
}
