#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66LeprechaunRulesRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66LeprechaunRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	// Tune later
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 BonusGoldOnKill = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0", ClampMax="1.0"))
	float ExtraLootBagChance = 0.10f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnabled = true;
};
