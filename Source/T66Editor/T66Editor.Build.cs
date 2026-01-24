// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class T66Editor : ModuleRules
{
    public T66Editor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",

                // ✅ Our runtime module (we include runtime DataAsset headers)
                "T66"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                // ✅ EKeys + keyboard/gamepad key symbols
                "InputCore",

                // ✅ UInputAction / UInputMappingContext
                "EnhancedInput",

                // ✅ Gameplay tags
                "GameplayTags",

                // ✅ Asset scanning
                "AssetRegistry",

                // ✅ Editor subsystem + editor APIs
                "UnrealEd",

                // ✅ SaveLoadedAsset
                "EditorScriptingUtilities",

                // ✅ Widgets (runtime)
                "UMG",
                "Slate",
                "SlateCore",

                // ✅ Widget Blueprint editor types (UWidgetBlueprint, etc.)
                "UMGEditor",

                // ✅ ToolMenus + Content Browser context
                "ToolMenus",
                "ContentBrowser"
            }
        );
    }
}
