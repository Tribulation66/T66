#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66AchievementRow.generated.h"

UENUM(BlueprintType)
enum class ET66AchievementTier : uint8
{
	Black UMETA(DisplayName="Black"),
	Red UMETA(DisplayName="Red"),
	Yellow UMETA(DisplayName="Yellow"),
	White UMETA(DisplayName="White"),
};

/**
 * Achievement definition.
 */
USTRUCT(BlueprintType)
struct T66_API FT66AchievementRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Achievement")
	ET66AchievementTier Tier = ET66AchievementTier::Black;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Achievement")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Achievement", meta=(MultiLine="true"))
	FText RequirementText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Achievement", meta=(ClampMin="0"))
	int32 RewardCoins = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Achievement", meta=(ClampMin="0"))
	int32 MaxProgress = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bHidden = false;
};
