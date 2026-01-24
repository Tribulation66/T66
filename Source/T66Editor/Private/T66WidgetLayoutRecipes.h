#pragma once

#include "CoreMinimal.h"

class UWidgetBlueprint;

/**
 * T66 Widget Layout Recipes
 * - Contains ONLY per-widget explicit layout stamping recipes.
 * - No generic recipes allowed.
 * - Each recipe must be "repair/add-only":
 *   - Never destroy or rearrange existing user layout.
 *   - Only create missing root/container when the WidgetTree is empty.
 *   - Only set exposed properties if they are empty/unset.
 *
 * The subsystem will:
 * - Gather selected widget blueprints
 * - Call TryApplyRecipe()
 * - Compile + Save
 */
namespace T66WidgetLayoutRecipes
{
	/**
	 * Tries to apply the recipe for the given widget blueprint based on its name.
	 * Returns true only if a recipe existed AND was applied successfully.
	 */
	bool TryApplyRecipe(UWidgetBlueprint* WidgetBP);
}
