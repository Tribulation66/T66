#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "T66RegistryTools.generated.h"

UCLASS()
class T66EDITOR_API UT66RegistryTools : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	// ✅ Fills DA_UIRegistry_Surfaces using WBP_* names
	UFUNCTION(CallInEditor, Category = "T66|RegistryTools")
	void FillSurfaceRegistry();

	// ✅ Fills DA_T66UIInputContexts using IMC_UI_* names
	UFUNCTION(CallInEditor, Category = "T66|RegistryTools")
	void FillInputContextRegistry();

	// ✅ Applies default UI keybinds into IMC_UI_* (keyboard + gamepad + Steam Deck)
	UFUNCTION(CallInEditor, Category = "T66|RegistryTools")
	void ApplyDefaultUIKeybinds();

	// ✅ Creates/repairs the required DA_UITheme_* assets and auto-fills safe defaults
	UFUNCTION(CallInEditor, Category = "T66|RegistryTools")
	void CreateOrRepairUIThemeAssets();
};
