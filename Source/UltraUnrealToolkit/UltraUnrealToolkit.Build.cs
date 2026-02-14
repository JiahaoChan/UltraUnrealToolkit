// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

using UnrealBuildTool;

public class UltraUnrealToolkit : ModuleRules
{
	public UltraUnrealToolkit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);

		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ApplicationCore",
				"Blutility",
				"ContentBrowser",
				"CoreUObject",
				"Engine",
				"InputCore",
				"LevelEditor",
				"MaterialEditor",
				"Projects",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"UMGEditor",
				"UnrealEd",
			}
			);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
