#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66StageLoopRulesRow.generated.h"

/**
 * Stage loop rules (DT-first). Typically 1 row: "Core".
 * This is NOT map layout. It is the run-loop tuning:
 * - stage max duration
 * - wave pacing / wave count range
 * - boss summon stone rules
 * - "boss exists when map is generated" rule
 */
USTRUCT(BlueprintType)
struct T66_API FT66StageLoopRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Maximum stage duration (seconds). Default 10 minutes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|StageLoop", meta=(ClampMin="1.0"))
	float StageMaxSeconds = 600.0f;

	/** Target time per wave (seconds). Default ~60. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|StageLoop|Waves", meta=(ClampMin="1.0"))
	float TargetWaveSeconds = 60.0f;

	/** Minimum wave count per stage. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|StageLoop|Waves", meta=(ClampMin="0", ClampMax="50"))
	int32 MinWaves = 6;

	/** Maximum wave count per stage. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|StageLoop|Waves", meta=(ClampMin="0", ClampMax="50"))
	int32 MaxWaves = 9;

	/**
	 * If true, boss can be summoned at any time via summoning stone (speedrun-friendly).
	 * If false, it only becomes available after waves or other conditions (later design).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|StageLoop|Boss")
	bool bBossSummonAllowedAnyTime = true;

	/** If true, summon is instant (no delay/cutscene gating). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|StageLoop|Boss")
	bool bBossSummonIsInstant = true;

	/**
	 * If true, the stage boss spawns/exists when the map is generated (already in its area).
	 * This matches your latest decision.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|StageLoop|Boss")
	bool bBossSpawnedAtMapGen = true;

	/**
	 * If true, interacting with Vendor/Gambler does NOT pause stage time.
	 * (You decided time continues; player is safe in a local force-field.)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|StageLoop|Time")
	bool bShopsDoNotPauseStageTime = true;
};
