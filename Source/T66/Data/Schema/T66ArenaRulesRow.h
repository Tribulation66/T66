#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66ArenaRulesRow.generated.h"

/**
 * Arena rules (DT-first). This is a "rules table" (usually 1 row: Core).
 *
 * Arena = pre-stage intermission that can appear before stages whose index ends with 0 or 5.
 * This is NOT map layout data; it's only tuning/rules knobs.
 */
USTRUCT(BlueprintType)
struct T66_API FT66ArenaRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Master toggle for the arena ruleset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Arena")
	bool bEnabled = true;

	/**
	 * Arena triggers when the *next* stage index ends with one of these digits.
	 * Default = 0 or 5 (your rule).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Arena")
	TArray<int32> TriggerStageEndDigits = { 0, 5 };

	/** Maximum number of bosses that can appear in the arena. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Arena", meta=(ClampMin="1", ClampMax="8"))
	int32 MaxBossCount = 4;

	/** Minimum bosses (kept simple; can tune later). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Arena", meta=(ClampMin="0", ClampMax="8"))
	int32 MinBossCount = 1;
};
