// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ProjectKR : ModuleRules
{
	public ProjectKR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Paper2D",
			"Landscape",
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
			"GameplayTasks",
			"LevelSequence",
			"UMG",
			//--------------------------------------------------//
			"SeedExt_Core",
			"CommonUI",
			"CommonInput",
			"GameplayTags",
			"EnhancedInput",
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
