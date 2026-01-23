#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "T66UISurfaceRegistryDataAsset.generated.h"

UCLASS(BlueprintType)
class T66_API UT66UISurfaceRegistryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Maps UI.Screen.*, UI.Modal.*, UI.Overlay.*, UI.Tooltip.* -> Widget Blueprint class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "T66|UI Registry")
	TMap<FGameplayTag, TSoftClassPtr<UUserWidget>> SurfaceTagToWidgetClass;

};
