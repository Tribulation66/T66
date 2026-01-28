#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66SpawnWeightingRow.generated.h"

UENUM(BlueprintType)
enum class ET66EnemySpawnClass : uint8
{
	Biped UMETA(DisplayName="Biped"),
	Quadruped UMETA(DisplayName="Quadruped"),
	Flying UMETA(DisplayName="Flying"),
	Unique UMETA(DisplayName="Unique"),
	MiniBoss UMETA(DisplayName="Mini-Boss"),
};

/**
 * Spawn director weighting adjustments.
 */
USTRUCT(BlueprintType)
struct T66_API FT66SpawnWeightingRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Director")
	ET66EnemySpawnClass SpawnClass = ET66EnemySpawnClass::Biped;

	// Multiplier applied to the selected class.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Director", meta=(ClampMin="0.0"))
	float ClassWeightMultiplier = 1.0f;

	// Multiplier applied to all other classes.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Director", meta=(ClampMin="0.0"))
	float OtherClassesMultiplier = 1.0f;

	// Chance the next wave is restricted to this class only.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Director", meta=(ClampMin="0.0", ClampMax="1.0"))
	float NextWaveOnlyClassChance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
