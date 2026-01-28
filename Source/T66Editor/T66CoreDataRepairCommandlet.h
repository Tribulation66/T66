#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66CoreDataRepairCommandlet.generated.h"

/**
 * Commandlet: Create/Repair core DT assets + DA_DataCatalog_Core.
 *
 * Usage:
 * UnrealEditor-Cmd.exe <Project>.uproject -run=T66CoreDataRepair -unattended -nop4
 */
UCLASS()
class T66EDITOR_API UT66CoreDataRepairCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66CoreDataRepairCommandlet();

	virtual int32 Main(const FString& Params) override;
};
