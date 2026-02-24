using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectKREditorTarget : TargetRules
{
	public ProjectKREditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ProjectKR");
		ExtraModuleNames.Add("ProjectKREditor");
		bUseUnityBuild = true;
		bUseAdaptiveUnityBuild = false;
	}
}