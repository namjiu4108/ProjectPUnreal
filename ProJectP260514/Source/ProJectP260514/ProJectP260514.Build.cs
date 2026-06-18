// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProJectP260514 : ModuleRules
{
	public ProJectP260514(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
            "Niagara"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ProJectP260514",
			"ProJectP260514/Variant_Platforming",
			"ProJectP260514/Variant_Platforming/Animation",
			"ProJectP260514/Variant_Combat",
			"ProJectP260514/Variant_Combat/AI",
			"ProJectP260514/Variant_Combat/Animation",
			"ProJectP260514/Variant_Combat/Gameplay",
			"ProJectP260514/Variant_Combat/Interfaces",
			"ProJectP260514/Variant_Combat/UI",
			"ProJectP260514/Variant_SideScrolling",
			"ProJectP260514/Variant_SideScrolling/AI",
			"ProJectP260514/Variant_SideScrolling/Gameplay",
			"ProJectP260514/Variant_SideScrolling/Interfaces",
			"ProJectP260514/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
