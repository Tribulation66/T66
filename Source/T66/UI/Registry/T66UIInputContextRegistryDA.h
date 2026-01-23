#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "T66UIInputContextRegistryDA.generated.h"

class UInputMappingContext;

UCLASS(BlueprintType)
class T66_API UT66UIInputContextRegistryDA : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Maps UI.Input.* -> Input Mapping Context assets
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "T66|UI Registry")
	TMap<FGameplayTag, TSoftObjectPtr<UInputMappingContext>> InputContextTagToIMC;

};
