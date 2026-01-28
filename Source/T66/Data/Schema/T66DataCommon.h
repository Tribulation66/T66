#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "T66DataCommon.generated.h"

/**
 * Canonical ID type for data-driven content.
 * We use Gameplay Tags as stable IDs (good for validation + registries + analytics/logging).
 */
USTRUCT(BlueprintType)
struct T66_API FT66IdTag
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "T66|ID")
	FGameplayTag Tag;

	bool IsValid() const { return Tag.IsValid(); }
};

/**
 * Weighted choice entry used by RNG systems (Luck / spawn / drops / jackpots).
 * Keep as weight (not probability) so designers can tune without renormalizing.
 */
USTRUCT(BlueprintType)
struct T66_API FT66WeightedChoice
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "T66|RNG", meta = (ClampMin = "0.0"))
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "T66|RNG")
	FT66IdTag ChoiceId;

	bool IsValid() const { return Weight > 0.0f && ChoiceId.IsValid(); }
};
