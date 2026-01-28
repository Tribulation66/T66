#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "T66RegistryTools.generated.h"

UCLASS()
class T66EDITOR_API UT66RegistryTools : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor, Category = "T66|RegistryTools")
	void FillSurfaceRegistry();

	UFUNCTION(CallInEditor, Category = "T66|RegistryTools")
	void FillInputContextRegistry();

	UFUNCTION(CallInEditor, Category = "T66|RegistryTools")
	void ApplyDefaultUIKeybinds();

	UFUNCTION(CallInEditor, Category = "T66|RegistryTools")
	void CreateOrRepairUIThemeAssets();

	// Core Data (DT-first)
	UFUNCTION(CallInEditor, Category = "T66|CoreData")
	void CreateOrRepairCoreDataTablesAndCatalog();

	UFUNCTION(CallInEditor, Category = "T66|CoreData")
	void CreateOrRepairCoreDataTablesAndCatalog_ForceOverwrite();

	// Returns true if validation passes
	static bool RunCoreDataRepair(bool bForceOverwrite);
};
