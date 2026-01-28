#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66LootBagDropRow.generated.h"

/**
 * Loot bag drop tuning.
 * RowName is the enemy archetype ID (or "Default" for global stage caps).
 */
USTRUCT(BlueprintType)
struct T66_API FT66LootBagDropRow : public FTableRowBase
{
	GENERATED_BODY()

	// Chance [0..1] to drop a loot bag on enemy death.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Loot", meta=(ClampMin="0.0", ClampMax="1.0"))
	float DropChance = 0.0f;

	// If true, this enemy always drops a bag (ignores DropChance).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Loot")
	bool bGuaranteedDrop = false;

	// Global stage caps (use on a "Default" row).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Loot", meta=(ClampMin="0"))
	int32 MinBagsPerStage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Loot", meta=(ClampMin="0"))
	int32 MaxBagsPerStage = 0;

	// Optional seeded expected range (use on "Default" row).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Loot", meta=(ClampMin="0"))
	int32 SeededExpectedMin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Loot", meta=(ClampMin="0"))
	int32 SeededExpectedMax = 0;
};
