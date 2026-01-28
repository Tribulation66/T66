#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66CreateSimpleMapCommandlet.generated.h"

/**
 * Creates a simple test map with floor and basic geometry.
 * Usage: -run=T66CreateSimpleMap
 */
UCLASS()
class T66EDITOR_API UT66CreateSimpleMapCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66CreateSimpleMapCommandlet();

	virtual int32 Main(const FString& Params) override;
};
