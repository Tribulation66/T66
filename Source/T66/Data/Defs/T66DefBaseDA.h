#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "T66/Data/Schema/T66DataCommon.h"
#include "T66DefBaseDA.generated.h"

/**
 * Base data definition for all gameplay content (Items, Enemies, Stages, etc.).
 * Data-first: no required meshes/SFX/VFX references here.
 *
 * Everything should have a stable ID tag so we can:
 * - validate uniqueness
 * - build registries
 * - log events / anti-cheat provenance
 */
UCLASS(BlueprintType)
class T66_API UT66DefBaseDA : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Canonical, stable ID for this definition (GameplayTag-based). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|ID")
	FT66IdTag Id;

	/** Human-readable name for UI/debug/logs (safe even if presentation is missing). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	FText DisplayName;

	/** Optional description text (safe even if presentation is missing). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text", meta=(MultiLine="true"))
	FText Description;

	/** Allows disabling a definition without deleting it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;

	UFUNCTION(BlueprintPure, Category="T66|ID")
	bool HasValidId() const { return Id.IsValid(); }
};
