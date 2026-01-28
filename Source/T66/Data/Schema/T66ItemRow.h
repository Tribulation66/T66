#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66ItemRow.generated.h"

/**
 * DT_Items row.
 * Canonical ID = the DataTable Row Name.
 */
USTRUCT(BlueprintType)
struct T66_API FT66ItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Economy", meta=(ClampMin="0"))
	int32 BuyValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Economy", meta=(ClampMin="0"))
	int32 SellValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Item", meta=(ClampMin="0.0"))
	float PowerGiven = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Item")
	bool bIsCursed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Item")
	bool bIsWhite = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	TArray<FText> EffectLines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
