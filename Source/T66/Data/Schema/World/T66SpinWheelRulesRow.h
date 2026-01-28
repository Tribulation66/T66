#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66SpinWheelRulesRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66SpinWheelRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	// Cost to play
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 CostGold = 50;

	// Rewards (parallel arrays; tune later)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int32> GoldRewards = { 25, 50, 100, 200 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> RewardWeights = { 0.40f, 0.35f, 0.20f, 0.05f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnabled = true;
};
