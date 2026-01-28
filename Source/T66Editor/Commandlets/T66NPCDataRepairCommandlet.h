#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66NPCDataRepairCommandlet.generated.h"

UCLASS()
class UT66NPCDataRepairCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66NPCDataRepairCommandlet();

	virtual int32 Main(const FString& Params) override;
};
