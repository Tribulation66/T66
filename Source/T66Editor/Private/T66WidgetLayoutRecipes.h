#pragma once

#include "CoreMinimal.h"

// Forward declarations
class UWidgetBlueprint;

/**
 * T66 Widget Layout Recipes
 *
 * Purpose:
 *  - Centralized, explicit "minimum layout" stamping for Widget Blueprints.
 *  - This is intentionally NOT generic auto-layout; each widget blueprint must have a known recipe.
 *
 * Coverage (as of this update):
 *  - Screens      (WBP_Screen_*)
 *  - Overlays     (WBP_Ov_*)
 *  - Modals       (WBP_Modal_*)
 *  - Tooltips     (WBP_Tooltip_*)
 *
 * Usage:
 *  - Called by UT66WidgetLayoutToolsSubsystem when applying recipes (Selected / All).
 *  - Safe behavior: add/repair only; do not overwrite existing user-built hierarchies.
 */
namespace T66WidgetLayoutRecipes
{
	/**
	 * Tries to apply the correct explicit minimum-layout recipe for the given Widget Blueprint.
	 *
	 * @return true if a recipe existed for this widget and was applied (or verified), false otherwise.
	 */
	bool TryApplyRecipe(UWidgetBlueprint* WidgetBP);
}
