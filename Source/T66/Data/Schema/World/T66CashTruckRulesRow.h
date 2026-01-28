#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66CashTruckRulesRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66CashTruckRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	// Hybrid behavior: on interact, chance to become an enemy instead of paying out
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0", ClampMax="1.0"))
	float MimicChance = 0.25f;

	// If NOT mimic: payout gold
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 GoldMin = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 GoldMax = 300;

	// If mimic: which enemy ID to spawn/activate (points at DT_Enemies RowName)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EnemyIdWhenMimic = FName(TEXT("Enemy.CashTruckMimic"));

	// Ram feel (tune later)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
	float RamDamage = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
	float KnockbackStrength = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnabled = true;
};
