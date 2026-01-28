#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66MainMenuWireCommandlet.generated.h"

/**
 * Wires Main Menu buttons to their Action/Route tags.
 * Usage: -run=T66MainMenuWire [-Force]
 */
UCLASS()
class T66EDITOR_API UT66MainMenuWireCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66MainMenuWireCommandlet();

	virtual int32 Main(const FString& Params) override;
};
