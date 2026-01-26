#pragma once

#include "CoreMinimal.h"

// Forward declarations
class UWidgetBlueprint;

/**
 * T66 Widget Layout Recipes (Contract Recipes)
 *
 * This file is now the "Contract Library" for Widget Blueprints.
 *
 * The Contracts Tool calls into this library to:
 *  - Ensure required named children exist (BG_Frame, VB_MainMenu, Button_NewGame, etc.)
 *  - Ensure required "pins exist for wiring later" (variables/properties if present)
 *  - (Later) enforce/repair WidgetID + SurfaceType properties on the class defaults
 *  - Optionally stamp a minimal layout when structure is missing
 *
 * Apply Modes:
 *  - SAFE (bForceOverride=false): Add/Repair only. Never delete or overwrite existing structure.
 *  - FORCE (bForceOverride=true): Delete + rebuild from recipe so new recipes can re-apply cleanly.
 *
 * NOTE:
 *  - The recipes are deterministic and explicit by Widget Blueprint name.
 *  - Your designers (you) can still freely resize/move things after stamping.
 */
namespace T66WidgetLayoutRecipes
{
	/**
	 * Tries to apply the correct contract recipe for the given Widget Blueprint.
	 *
	 * @param WidgetBP        Widget Blueprint to stamp/repair.
	 * @param bForceOverride  SAFE=false (repair/add only), FORCE=true (delete + rebuild)
	 *
	 * @return true if a recipe existed for this widget and was applied, false otherwise.
	 */
	bool TryApplyRecipe(UWidgetBlueprint* WidgetBP, bool bForceOverride);

	// Small convenience helpers (do not require cpp implementation)
	inline bool TryApplyRecipe_Safe(UWidgetBlueprint* WidgetBP)
	{
		return TryApplyRecipe(WidgetBP, /*bForceOverride=*/false);
	}

	inline bool TryApplyRecipe_Force(UWidgetBlueprint* WidgetBP)
	{
		return TryApplyRecipe(WidgetBP, /*bForceOverride=*/true);
	}
}
