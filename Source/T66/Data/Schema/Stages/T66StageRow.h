#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66StageRow.generated.h"

UENUM(BlueprintType)
enum class ET66StageKind : uint8
{
	Normal UMETA(DisplayName="Normal"),
	MiniBoss UMETA(DisplayName="Mini-Boss"),
	Boss UMETA(DisplayName="Boss"),
	EpicBoss UMETA(DisplayName="Epic Boss"),
	Espada UMETA(DisplayName="Espada"),
	Final UMETA(DisplayName="Final"),
};

/**
 * Stage roster / boss schedule.
 * RowName can be "Stage_01", "Stage_02", etc.
 */
USTRUCT(BlueprintType)
struct T66_API FT66StageRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Stage", meta=(ClampMin="1"))
	int32 StageNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Stage")
	ET66StageKind StageKind = ET66StageKind::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Stage")
	FText DisplayName;

	// Boss ID (row name in DT_Bosses), if applicable.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Stage")
	FName BossId = NAME_None;
};
