#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66MainMenuDumpCommandlet.generated.h"

/**
 * Dumps WBP_Screen_MainMenu widget hierarchy and layout info.
 * Usage: -run=T66MainMenuDump
 */
UCLASS()
class T66EDITOR_API UT66MainMenuDumpCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66MainMenuDumpCommandlet();

	virtual int32 Main(const FString& Params) override;
};
