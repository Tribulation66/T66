#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66IdolRow.generated.h"

/**
 * Idol definition and tiered behavior.
 */
USTRUCT(BlueprintType)
struct T66_API FT66IdolRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Idol")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Idol", meta=(MultiLine="true"))
	FText Tier1Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Idol", meta=(MultiLine="true"))
	FText Tier2Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Idol", meta=(MultiLine="true"))
	FText Tier3Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
