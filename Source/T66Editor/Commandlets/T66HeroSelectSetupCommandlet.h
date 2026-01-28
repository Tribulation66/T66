#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "T66HeroSelectSetupCommandlet.generated.h"

UCLASS()
class T66EDITOR_API UT66HeroSelectSetupCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UT66HeroSelectSetupCommandlet();
	virtual int32 Main(const FString& Params) override;

private:
	void SetupHeroSelectSolo();
	void SetupCompanionSelect();
};
