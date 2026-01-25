#pragma once

#include "EditorSubsystem.h"
#include "T66WidgetLayoutToolsSubsystem.generated.h"

/**
 * UT66WidgetLayoutToolsSubsystem
 * Editor-only utilities for repairing/stamping Widget Blueprints.
 *
 * Repair/Add-only policy:
 * - Never deletes
 * - Never repositions existing widgets
 * - Only adds missing components/vars by exact name
 */
UCLASS()
class T66EDITOR_API UT66WidgetLayoutToolsSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Creates/repairs WBP_Comp_Button_Action:
	 * - Adds required exposed variables (ControlID/ActionTag/RouteTag/LabelText)
	 * - Adds minimal widget tree if empty (Button_Root -> Text_Label)
	 */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void CreateOrRepairUIButtonComponents();

	/**
	 * Build minimum layouts for selected Widget Blueprints (explicit recipes only).
	 * - Only runs for known widget names (no generic)
	 * - Only stamps if the widget has NO root yet (empty designer)
	 */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForSelectedWidgetBlueprints();

	/**
	 * Build minimum layouts for ALL surface Widget Blueprints.
	 * Searches the canonical surface folders and stamps known widgets using explicit recipes only.
	 */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllSurfaceWidgetBlueprints();

	/**
	 * Build minimum layouts for ALL component Widget Blueprints.
	 * Searches the canonical component folders and stamps known widgets using explicit recipes only.
	 */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllComponentWidgetBlueprints();

	/** Build minimum layouts for ALL components in /UI/Components/Button (explicit recipes only). */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllButtonComponentWidgetBlueprints();

	/** Build minimum layouts for ALL components in /UI/Components/Text (explicit recipes only). */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllTextComponentWidgetBlueprints();

	/** Build minimum layouts for ALL components in /UI/Components/UI_Blocks (explicit recipes only). */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllUIBlocksComponentWidgetBlueprints();

	/** Build minimum layouts for ALL components in /UI/Components/Utility_UI (explicit recipes only). */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllUtilityUIComponentWidgetBlueprints();

	/** Build minimum layouts for ALL WBP_Screen_* screens (explicit recipes only). */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllScreenWidgetBlueprints();

	/** Build minimum layouts for ALL WBP_Ov_* overlays (explicit recipes only). */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllOverlayWidgetBlueprints();

	/** Build minimum layouts for ALL WBP_Modal_* modals (explicit recipes only). */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllModalWidgetBlueprints();

	/** Build minimum layouts for ALL WBP_Tooltip_* tooltips (explicit recipes only). */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllTooltipWidgetBlueprints();
};
