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
	 * Build minimum layouts for ALL Screen surfaces (WBP_Screen_*) using explicit recipes only.
	 * - Only stamps if the widget has NO root yet (empty designer)
	 */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllScreenWidgetBlueprints();

	/** Build minimum layouts for ALL Overlay surfaces (WBP_Ov_*) using explicit recipes only. */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllOverlayWidgetBlueprints();

	/** Build minimum layouts for ALL Modal surfaces (WBP_Modal_*) using explicit recipes only. */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllModalWidgetBlueprints();

	/** Build minimum layouts for ALL Tooltip surfaces (WBP_Tooltip_*) using explicit recipes only. */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllTooltipWidgetBlueprints();

	/**
	 * Convenience: Build minimum layouts for ALL surfaces (Screens + Overlays + Modals + Tooltips).
	 * Uses explicit recipes only.
	 */
	UFUNCTION(CallInEditor, Category = "T66|WidgetLayoutTools")
	void BuildMinimumLayoutsForAllSurfaceWidgetBlueprints();
};
