#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66AttackPatternRow.generated.h"

/**
 * DT_AttackPatterns row.
 * Canonical ID = RowName.
 */
USTRUCT(BlueprintType)
struct T66_API FT66AttackPatternRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Tuning", meta=(ClampMin="0.0"))
	float CooldownSeconds = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Tuning", meta=(ClampMin="0.0"))
	float Range = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
