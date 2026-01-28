#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66CompanionRow.generated.h"

/**
 * DT_Companions row.
 * Canonical ID = the DataTable Row Name.
 */
USTRUCT(BlueprintType)
struct T66_API FT66CompanionRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text", meta=(MultiLine="true"))
	FText PassiveDescription;

	/** Optional: row ID in a future DT_PassiveEffects (if you formalize them) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Companion")
	FName PassiveEffectId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
