#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66HeroRow.generated.h"

/**
 * Hero roster entry (auto attack + ultimate definition hooks).
 */
USTRUCT(BlueprintType)
struct T66_API FT66HeroRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Hero")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Hero", meta=(MultiLine="true"))
	FText AutoAttackDescription;

	// Ultimate ID (row name in DT_Ultimates).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Hero")
	FName UltimateId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Hero", meta=(MultiLine="true"))
	FText UltimateDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
