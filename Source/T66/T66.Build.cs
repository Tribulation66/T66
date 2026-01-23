// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class T66 : ModuleRules
{
    public T66(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "AIModule",
            "StateTreeModule",
            "GameplayStateTreeModule",
            "GameplayTags",
            "UMG",
            "Slate",
            "SlateCore" // ✅ REQUIRED for FMargin (fixes linker error)
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.AddRange(new string[]
        {
            "T66",

            // ✅ Correct path (relative to Source/T66)
            "T66/UI/Registry",

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
    }
}
