#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66MiasmaRulesRow.generated.h"

/**
 * Miasma rules (DT-first). This is a "rules table" (usually few rows),
 * used to tune timing + lethality without touching code.
 *
 * Suggested rows:
 *  - Core (baseline)
 *  - Easy / Medium / Hard (optional later)
 */
USTRUCT(BlueprintType)
struct T66_API FT66MiasmaRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	/** When miasma begins rising (seconds since stage start). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Miasma")
	float StartTimeSeconds = 420.0f; // ~7:00

	/** When the map is fully covered by miasma (seconds since stage start). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Miasma")
	float FullCoverTimeSeconds = 600.0f; // 10:00

	/** If true, touching miasma is instantly lethal. If false, use DamagePerSecond. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Miasma")
	bool bInstantKillOnContact = true;

	/** Damage per second when bInstantKillOnContact=false (ignored otherwise). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Miasma", meta=(EditCondition="!bInstantKillOnContact", ClampMin="0.0"))
	float DamagePerSecond = 9999.0f;
};
