#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66TricksterRulesRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66TricksterRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	// Bible: appears on normal stages (NOT ending in 5 or 0). :contentReference[oaicite:10]{index=10}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bOnlyOnNormalStages = true;

	// Bible: delivers Cowardice Gate at boss approach entrance. :contentReference[oaicite:11]{index=11}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDeliversCowardiceGate = true;
};
