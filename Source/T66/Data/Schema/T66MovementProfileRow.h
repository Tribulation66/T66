#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66MovementProfileRow.generated.h"

/**
 * DT_MovementProfiles row.
 * Canonical ID = RowName.
 */
USTRUCT(BlueprintType)
struct T66_API FT66MovementProfileRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Tuning", meta=(ClampMin="0.0"))
	float PreferredDistance = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
