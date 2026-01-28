#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66ButtonComponentFixCommandlet.generated.h"

/**
 * Fixes WBP_Comp_Button_Action to have visible content.
 * Usage: -run=T66ButtonComponentFix
 */
UCLASS()
class T66EDITOR_API UT66ButtonComponentFixCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66ButtonComponentFixCommandlet();

	virtual int32 Main(const FString& Params) override;
};
