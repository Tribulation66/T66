// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class T66 : ModuleRules
{
	public T66(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"T66",
			"T66/Variant_Platforming",
			"T66/Variant_Platforming/Animation",
			"T66/Variant_Combat",
			"T66/Variant_Combat/AI",
			"T66/Variant_Combat/Animation",
			"T66/Variant_Combat/Gameplay",
			"T66/Variant_Combat/Interfaces",
			"T66/Variant_Combat/UI",
			"T66/Variant_SideScrolling",
			"T66/Variant_SideScrolling/AI",
			"T66/Variant_SideScrolling/Gameplay",
			"T66/Variant_SideScrolling/Interfaces",
			"T66/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
