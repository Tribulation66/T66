#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66DodgeRulesRow.generated.h"

/**
 * Dodge rules (DT-first). Typically 1 row: "Core".
 * Used for End Run review + anti-cheat suspicion tuning (NOT the wiring/logic).
 */
USTRUCT(BlueprintType)
struct T66_API FT66DodgeRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Master toggle for dodge scoring. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Dodge")
	bool bEnabled = true;

	/** Window sizes (seconds) used to grade dodge quality (tunable; logic later). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Dodge|Windows", meta=(ClampMin="0.0"))
	float PerfectWindowSeconds = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Dodge|Windows", meta=(ClampMin="0.0"))
	float GreatWindowSeconds = 0.14f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Dodge|Windows", meta=(ClampMin="0.0"))
	float GoodWindowSeconds = 0.22f;

	/** End-run rating weights (relative contributions). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Dodge|Score", meta=(ClampMin="0.0"))
	float PerfectWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Dodge|Score", meta=(ClampMin="0.0"))
	float GreatWeight = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Dodge|Score", meta=(ClampMin="0.0"))
	float GoodWeight = 0.3f;

	/** Anti-cheat suspicion knobs (tunable thresholds; enforcement later). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Dodge|AntiCheat", meta=(ClampMin="0.0", ClampMax="1.0"))
	float MaxPerfectRateBeforeFlag = 0.90f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Dodge|AntiCheat", meta=(ClampMin="0"))
	int32 MaxConsecutivePerfectBeforeFlag = 30;

	/** Optional: require a minimum number of "exposures" before evaluating rates. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Dodge|AntiCheat", meta=(ClampMin="0"))
	int32 MinEvaluatedDodgeCount = 50;
};
