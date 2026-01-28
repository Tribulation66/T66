#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66MainMenuCenterCommandlet.generated.h"

/**
 * Centers main menu buttons in WBP_Screen_MainMenu.
 * Usage: -run=T66MainMenuCenter
 */
UCLASS()
class T66EDITOR_API UT66MainMenuCenterCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66MainMenuCenterCommandlet();

	virtual int32 Main(const FString& Params) override;
};
