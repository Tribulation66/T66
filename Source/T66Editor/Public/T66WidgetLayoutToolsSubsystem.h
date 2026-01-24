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
};
