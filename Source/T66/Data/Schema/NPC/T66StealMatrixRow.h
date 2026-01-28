#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66StealMatrixRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66StealMatrixRow : public FTableRowBase
{
	GENERATED_BODY()

	// AttemptAmountTier -> (AngerThresholdIncrease, BaseSuccessDifficulty). :contentReference[oaicite:13]{index=13}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 AttemptAmountGold = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 AngerThresholdIncrease = 50;

	// Interpreted as baseline difficulty in [0..1] (tune later)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0", ClampMax="1.0"))
	float BaseSuccessDifficulty = 0.35f;
};
