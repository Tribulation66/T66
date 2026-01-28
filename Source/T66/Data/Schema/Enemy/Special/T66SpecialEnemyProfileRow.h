#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66SpecialEnemyProfileRow.generated.h"

UENUM(BlueprintType)
enum class ET66SpecialEnemyType : uint8
{
	GoblinThief UMETA(DisplayName="Goblin Thief"),
	Leprechaun  UMETA(DisplayName="Leprechaun"),
};

USTRUCT(BlueprintType)
struct T66_API FT66SpecialEnemyProfileRow : public FTableRowBase
{
	GENERATED_BODY()

	// RowName is canonical ID (e.g., SpecialEnemy.GoblinThief)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ET66SpecialEnemyType Type = ET66SpecialEnemyType::GoblinThief;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	// Points at DT_Enemies RowName (the actual enemy definition you’ll run/spawn)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BaseEnemyId = NAME_None;

	// Optional hook into your luck system (DT_LuckSources RowName)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName LuckSourceId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnabled = true;
};
