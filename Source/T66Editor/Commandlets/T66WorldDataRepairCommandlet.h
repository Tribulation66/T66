#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66WorldDataRepairCommandlet.generated.h"

UCLASS()
class UT66WorldDataRepairCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66WorldDataRepairCommandlet();
	virtual int32 Main(const FString& Params) override;
};
