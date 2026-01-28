#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66CoopRulesRow.generated.h"

/**
 * Co-op rules (DT-first). Typically 1 row: "Core".
 * Focus: revive/down mechanics and any co-op-only tuning knobs.
 * (Wiring/logic comes later; this is only tunables.)
 */
USTRUCT(BlueprintType)
struct T66_API FT66CoopRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Master toggle for co-op revive system. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Coop")
	bool bReviveEnabled = true;

	/** Time to revive a downed player (seconds). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Coop|Revive", meta=(ClampMin="0.0"))
	float ReviveTimeSeconds = 3.0f;

	/** Invulnerability granted after being revived (seconds). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Coop|Revive", meta=(ClampMin="0.0"))
	float PostReviveInvulnSeconds = 1.0f;

	/** If >0, limits downs per stage; 0 = unlimited. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Coop|Down", meta=(ClampMin="0"))
	int32 MaxDownsPerStage = 0;

	/** If >0, limits downs per run; 0 = unlimited. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Coop|Down", meta=(ClampMin="0"))
	int32 MaxDownsPerRun = 0;
};
