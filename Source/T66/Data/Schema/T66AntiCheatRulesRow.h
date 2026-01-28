#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66AntiCheatRulesRow.generated.h"

/**
 * Anti-cheat rules (DT-first). Typically 1 row: "Core".
 * This is NOT the full anti-cheat implementation; it is tunable thresholds + hard rules.
 *
 * We keep types lightweight (bool/int/float/FName arrays) so we don't pull extra dependencies.
 */
USTRUCT(BlueprintType)
struct T66_API FT66AntiCheatRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	/** If LuckRating exceeds this value, flag "Too Lucky". */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|AntiCheat|Luck", meta=(ClampMin="0.0"))
	float LuckTooLuckyThreshold = 100.0f;

	/** If true, the run is flagged when LuckTooLuckyThreshold is exceeded. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|AntiCheat|Luck")
	bool bEnableTooLuckyFlag = true;

	/**
	 * Forbidden events = cheating certainty (no appeal).
	 * Store as FName ids for now (e.g., "Forbidden.ItemsNotFromValidSource").
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|AntiCheat|Forbidden")
	TArray<FName> ForbiddenEventIds;

	/** If true, any forbidden event immediately disqualifies the run. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|AntiCheat|Forbidden")
	bool bForbiddenEventsAreCertainty = true;

	/**
	 * High-level caps (optional tuning knobs; can be expanded later).
	 * Keep them off by default until we decide exact models.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|AntiCheat|Caps")
	bool bEnableCaps = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|AntiCheat|Caps", meta=(EditCondition="bEnableCaps", ClampMin="0"))
	int32 MaxScorePerStage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|AntiCheat|Caps", meta=(EditCondition="bEnableCaps", ClampMin="0"))
	int32 MaxGoldPerStageNonGambling = 0;
};
