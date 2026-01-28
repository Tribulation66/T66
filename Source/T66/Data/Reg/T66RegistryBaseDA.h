#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "T66/Data/Defs/T66DefBaseDA.h"
#include "T66RegistryBaseDA.generated.h"

/**
 * Base registry for data-driven content.
 * A registry is the canonical "what exists" list used for validation, packaging, and generation.
 *
 * Note: We intentionally use soft object pointers so presentation can be added later
 * without hard-load requirements.
 */
UCLASS(BlueprintType)
class T66_API UT66RegistryBaseDA : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Definitions included in this registry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Registry")
	TArray<TSoftObjectPtr<UT66DefBaseDA>> Defs;

	/** Optional: allow registry to be disabled temporarily. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Registry")
	bool bEnabled = true;
};
