#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66GambleOutcomeRow.generated.h"

UENUM(BlueprintType)
enum class ET66GambleOutcomeTier : uint8
{
	Loss UMETA(DisplayName="Loss"),
	Win UMETA(DisplayName="Win"),
	Jackpot UMETA(DisplayName="Jackpot"),
};

/**
 * Gambler outcome probability table (win/loss/jackpot tiers).
 */
USTRUCT(BlueprintType)
struct T66_API FT66GambleOutcomeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Gamble")
	ET66GambleOutcomeTier OutcomeTier = ET66GambleOutcomeTier::Loss;

	// Weight used for the outcome roll.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Gamble", meta=(ClampMin="0.0"))
	float Weight = 1.0f;

	// Multiplier applied to the wager when this outcome occurs.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Gamble", meta=(ClampMin="0.0"))
	float PayoutMultiplier = 0.0f;

	// Optional flat bonus on top of multiplier.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Gamble", meta=(ClampMin="0"))
	int32 FlatBonusGold = 0;
};
