using UnrealBuildTool;

public class ProjectKRClient : ModuleRules
{
    public ProjectKRClient(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Paper2D",
            "Landscape",
            "Json",
            "JsonUtilities",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "ProjectKR",
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Engine",
            "InputCore",
            "Slate",
            "SlateCore",
            "ApplicationCore",
            "Navmesh",
            "NavigationSystem",
            "AIModule",
            "LevelSequence",
            "UMG",
            //--------------------------------------------------//
            "SeedExt_Core",
            "CommonUI",
            "CommonInput",
            "GameplayTags",
            "EnhancedInput",
        });
    }
}