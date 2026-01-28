#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66CheatMatrixRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66CheatMatrixRow : public FTableRowBase
{
	GENERATED_BODY()

	// AttemptAmountTier -> (AngerThresholdIncrease, BaseSuccessDifficulty). :contentReference[oaicite:14]{index=14}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 AttemptAmountGold = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 AngerThresholdIncrease = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0", ClampMax="1.0"))
	float BaseSuccessDifficulty = 0.35f;
};
