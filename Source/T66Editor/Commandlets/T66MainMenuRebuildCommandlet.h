#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66MainMenuRebuildCommandlet.generated.h"

/**
 * Force rebuilds WBP_Screen_MainMenu layout and buttons.
 * Usage: -run=T66MainMenuRebuild
 */
UCLASS()
class T66EDITOR_API UT66MainMenuRebuildCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66MainMenuRebuildCommandlet();

	virtual int32 Main(const FString& Params) override;
};
