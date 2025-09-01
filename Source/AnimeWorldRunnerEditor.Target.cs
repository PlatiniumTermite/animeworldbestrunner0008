using UnrealBuildTool;

public class AnimeWorldRunnerEditorTarget : TargetRules
{
	public AnimeWorldRunnerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("AnimeWorldRunner");
	}
}
