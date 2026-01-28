#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66ScreenLayoutNormalizeCommandlet.generated.h"

/**
 * Normalizes layout/visibility for all WBP_Screen_* widgets.
 * Usage: -run=T66ScreenLayoutNormalize
 */
UCLASS()
class T66EDITOR_API UT66ScreenLayoutNormalizeCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66ScreenLayoutNormalizeCommandlet();

	virtual int32 Main(const FString& Params) override;
};
