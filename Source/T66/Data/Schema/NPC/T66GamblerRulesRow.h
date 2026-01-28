#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66GamblerRulesRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66GamblerRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	// Bible: games v1.0 = Rock Paper Scissors, Find the Ball, Coin Flip. :contentReference[oaicite:7]{index=7}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> GameIds = { FName(TEXT("RockPaperScissors")), FName(TEXT("FindTheBall")), FName(TEXT("CoinFlip")) };

	// Bible: stage anger threshold rolls per stage; tuning target 700–1400g. :contentReference[oaicite:8]{index=8}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 AngerThresholdMin = 700;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 AngerThresholdMax = 1400;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowCheating = true;
};
