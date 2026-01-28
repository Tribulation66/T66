#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66LoanSharkRulesRow.generated.h"

/**
 * Loan Shark tier rules (Debt -> scaling).
 */
USTRUCT(BlueprintType)
struct T66_API FT66LoanSharkRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|LoanShark", meta=(ClampMin="0"))
	int32 DebtMin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|LoanShark", meta=(ClampMin="0"))
	int32 DebtMax = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|LoanShark", meta=(ClampMin="0.0"))
	float HealthMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|LoanShark", meta=(ClampMin="0.0"))
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|LoanShark", meta=(ClampMin="0.0"))
	float SpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|LoanShark", meta=(ClampMin="0"))
	int32 RewardGold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|LoanShark", meta=(ClampMin="0"))
	int32 RewardScore = 0;
};
