#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66ScreenSetupCommandlet.generated.h"

/**
 * Sets up all UI screens with proper buttons and layout.
 * Usage: -run=T66ScreenSetup
 */
UCLASS()
class T66EDITOR_API UT66ScreenSetupCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66ScreenSetupCommandlet();

	virtual int32 Main(const FString& Params) override;

private:
	void SetupPartySizePicker();
	void SetupHeroSelectSolo();
	void SetupHeroSelectCoop();
	void SetupSaveSlots();
	void SetupAchievements();
	void SetupAccountStatus();
};
