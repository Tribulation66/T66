#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66MainMenuSetupCommandlet.generated.h"

/**
 * Creates an empty Main Menu map and sets it as default.
 * Usage: -run=T66MainMenuSetup
 */
UCLASS()
class T66EDITOR_API UT66MainMenuSetupCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66MainMenuSetupCommandlet();

	virtual int32 Main(const FString& Params) override;
};
