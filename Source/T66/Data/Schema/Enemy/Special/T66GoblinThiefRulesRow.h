#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66GoblinThiefRulesRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66GoblinThiefRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	// Tune later
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 StealGoldAmount = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
	float EscapeTimeSeconds = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0", ClampMax="1.0"))
	float DropBackPercentOnKill = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnabled = true;
};
