#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66LuckSourceRow.generated.h"

/**
 * DT_LuckSources row.
 * Canonical ID = RowName.
 * This is a descriptor for RNG sources that feed Luck Rating / anti-cheat later.
 */
USTRUCT(BlueprintType)
struct T66_API FT66LuckSourceRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Luck", meta=(ClampMin="0"))
	int32 ExpectedMinPerStage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Luck", meta=(ClampMin="0"))
	int32 ExpectedMaxPerStage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Luck")
	float LuckImpactValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
