using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectKREditorTarget : TargetRules
{
	public ProjectKREditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("ProjectKR");
		ExtraModuleNames.Add("ProjectKRClient");
		ExtraModuleNames.Add("ProjectKREditor");
		bUseUnityBuild = true;
		bUseAdaptiveUnityBuild = false;
	}
}