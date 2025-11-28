// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SeedExt_Core : ModuleRules
{
	public SeedExt_Core(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				
		});
		
		PrivateIncludePaths.AddRange(
			new string[] {
				
		});
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]{
			"Engine",
			"InputCore",
			"Slate",
			"SlateCore",
			"ApplicationCore",
			"AnimGraphRuntime",
			"Navmesh",
			"NavigationSystem",
			"AIModule",
			"GameplayTasks",
			"LevelSequence",
			"MovieScene",
			"MovieSceneTracks",
			"CinematicCamera",
			"UMG",
			"CommonUI",
			"CommonInput",
			"GameplayTags",
			"Renderer",
			"RHI",
			"RenderCore",
			"OpenSSL",
			"Json",
			"JsonUtilities",
			"Niagara",
			"EnhancedInput",
			//--------------------------------------------------//
			//"KawaiiPhysics",
			//"FileDownloader",
			"GameplayDebugger",
		});
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
