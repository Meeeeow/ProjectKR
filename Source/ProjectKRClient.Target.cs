using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectKRClientTarget : TargetRules
{
	public ProjectKRClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("ProjectKR");
		ExtraModuleNames.Add("ProjectKRClient");
		bUseUnityBuild = true;
		bUseAdaptiveUnityBuild = false;
	}
}
