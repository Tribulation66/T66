#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "T66SimpleGameMode.generated.h"

/**
 * Simple game mode that spawns the T66SimpleCharacter.
 * No Blueprint dependencies - pure C++.
 */
UCLASS()
class T66_API AT66SimpleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AT66SimpleGameMode();
};
