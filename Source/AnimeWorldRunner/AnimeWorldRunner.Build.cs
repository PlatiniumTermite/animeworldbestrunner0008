using UnrealBuildTool;

public class AnimeWorldRunner : ModuleRules
{
	public AnimeWorldRunner(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"Slate",
			"SlateCore",
			"UMG",
			"RHI",
			"RenderCore",
			"ShaderCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Optimize for size in shipping builds
		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
		
		// Android specific settings
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateDependencyModuleNames.Add("AndroidRuntimeSettings");
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
	}
}
