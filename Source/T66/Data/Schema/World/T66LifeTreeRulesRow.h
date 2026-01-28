#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66LifeTreeRulesRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66LifeTreeRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	// Keep this flexible for now; we’ll firm it up once we wire gameplay.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 CostGold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> EffectLines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnabled = true;
};
