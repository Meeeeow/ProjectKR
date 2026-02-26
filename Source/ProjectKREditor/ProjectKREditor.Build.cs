using UnrealBuildTool;

public class ProjectKREditor : ModuleRules
{
	public ProjectKREditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core",
			"CoreUObject",
			"Engine",
			"UnrealEd",
			"InputCore",
			"Paper2D",
			"Slate",
			"SlateCore",
			"SeedExt_Core",
			"ProjectKR",
			"Landscape",
			"LandscapeEditor",
			"LandscapeEditorUtilities",
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"Engine",
			"InputCore",
			"Slate",
			"SlateCore",
			"ApplicationCore",
			"Navmesh",
			"NavigationSystem",
			"AIModule",
			"GameplayTasks",
			"LevelSequence",
			"UMG",
			//--------------------------------------------------//
			"CommonUI",
			"CommonInput",
			"GameplayTags",
			"EnhancedInput",
		});
	}
}