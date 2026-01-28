#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "T66MenuGameMode.generated.h"

/**
 * Game mode for the main menu - doesn't spawn a player pawn
 */
UCLASS()
class T66_API AT66MenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AT66MenuGameMode();
};
