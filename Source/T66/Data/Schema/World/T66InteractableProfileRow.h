#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66InteractableProfileRow.generated.h"

UENUM(BlueprintType)
enum class ET66InteractableType : uint8
{
	SpinWheel UMETA(DisplayName="Spin Wheel"),
	CashTruck UMETA(DisplayName="Cash Truck"),
	LifeTree  UMETA(DisplayName="Life Tree"),
};

UENUM(BlueprintType)
enum class ET66InteractableBehavior : uint8
{
	AlwaysInteractable UMETA(DisplayName="Always Interactable"),
	HybridCanBecomeEnemy UMETA(DisplayName="Hybrid: Can Become Enemy"),
};

USTRUCT(BlueprintType)
struct T66_API FT66InteractableProfileRow : public FTableRowBase
{
	GENERATED_BODY()

	// RowName is the canonical ID (e.g., Interactable.SpinWheel)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ET66InteractableType Type = ET66InteractableType::SpinWheel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ET66InteractableBehavior Behavior = ET66InteractableBehavior::AlwaysInteractable;

	// Optional hook into your luck system (points at DT_LuckSources RowName)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName LuckSourceId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnabled = true;
};
