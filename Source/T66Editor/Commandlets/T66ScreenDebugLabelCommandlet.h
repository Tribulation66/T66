#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66ScreenDebugLabelCommandlet.generated.h"

/**
 * Adds a debug text label to all WBP_Screen_* widgets.
 * Usage: -run=T66ScreenDebugLabel
 */
UCLASS()
class T66EDITOR_API UT66ScreenDebugLabelCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66ScreenDebugLabelCommandlet();

	virtual int32 Main(const FString& Params) override;
};
