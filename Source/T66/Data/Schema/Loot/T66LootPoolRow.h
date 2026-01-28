#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66LootPoolRow.generated.h"

/**
 * Loot pool entry (item roll within rarity).
 */
USTRUCT(BlueprintType)
struct T66_API FT66LootPoolRow : public FTableRowBase
{
	GENERATED_BODY()

	// Rarity ID this entry belongs to (e.g., Common, White).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Loot")
	FName RarityId = NAME_None;

	// Item ID (row name in DT_Items).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Loot")
	FName ItemId = NAME_None;

	// Weight within the rarity pool.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Loot", meta=(ClampMin="0.0"))
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
