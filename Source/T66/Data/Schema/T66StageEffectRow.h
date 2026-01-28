#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66StageEffectRow.generated.h"

/**
 * DT_StageEffects row.
 * Canonical ID = the DataTable Row Name.
 */
USTRUCT(BlueprintType)
struct T66_API FT66StageEffectRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	TArray<FText> Notes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
