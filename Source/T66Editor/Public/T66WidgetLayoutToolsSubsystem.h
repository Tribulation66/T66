#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "T66WidgetLayoutToolsSubsystem.generated.h"

/**
 * UT66WidgetLayoutToolsSubsystem
 * Editor-only utilities for stamping/repairing Widget Blueprints.
 *
 * FINAL POLICY (Contracts):
 * - SAFE: Add/Repair only (no overwriting existing structure)
 * - FORCE: Delete + rebuild from recipe (apply newest contracts exactly)
 */
UCLASS()
class T66EDITOR_API UT66WidgetLayoutToolsSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Create/Repair UI Widget Contracts for the Widget Blueprints selected in the Content Browser.
	 * SAFE MODE:
	 * - Adds missing required child widgets / components
	 * - Ensures required contract properties exist (inherited)
	 * - Sets/repairs SurfaceType + WidgetID where applicable
	 * - Does NOT remove existing widgets or overwrite layout
	 */
	UFUNCTION(CallInEditor, Category = "T66|UI|Contracts")
	void CreateOrRepairUIWidgetContracts_SelectedWidgets_Safe();

	/**
	 * Create/Repair UI Widget Contracts for the Widget Blueprints selected in the Content Browser.
	 * FORCE OVERWRITE MODE:
	 * - Deletes + rebuilds widget tree from recipe
	 * - Applies the newest contract structure/layout exactly
	 * - Use when recipes change and you want them re-applied
	 */
	UFUNCTION(CallInEditor, Category = "T66|UI|Contracts")
	void CreateOrRepairUIWidgetContracts_SelectedWidgets_ForceOverwrite();

	// ---------------------------------------------------------------------
	// Legacy / transitional tools (kept temporarily so we don't break builds).
	// These will be folded into the Contracts system and can be removed later.
	// ---------------------------------------------------------------------

	/**
	 * Creates/repairs WBP_Comp_Button_Action:
	 * - Adds required exposed variables (ControlID/ActionTag/RouteTag/LabelText)
	 * - Adds minimal widget tree if empty (Button_Root -> Text_Label)
	 *
	 * NOTE: This will be absorbed into the Contracts system.
	 */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void CreateOrRepairUIButtonComponents();

	/**
	 * Build minimum layouts for selected Widget Blueprints.
	 * - Uses explicit recipes (by widget name/path) from T66WidgetLayoutRecipes
	 * - Force override: rebuilds the widget tree for each selected widget blueprint
	 *
	 * NOTE: This will be absorbed into the Contracts system.
	 */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForSelectedWidgetBlueprints();
};
