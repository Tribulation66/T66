#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66LootBagRarityRow.generated.h"

/**
 * Loot bag rarity odds table.
 * RowName is the rarity ID (e.g., Common, Rare, White).
 */
USTRUCT(BlueprintType)
struct T66_API FT66LootBagRarityRow : public FTableRowBase
{
	GENERATED_BODY()

	// Weight used for rarity roll (higher = more likely).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Loot", meta=(ClampMin="0.0"))
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
