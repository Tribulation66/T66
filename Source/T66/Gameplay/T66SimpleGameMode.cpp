#include "T66SimpleGameMode.h"
#include "T66SimpleCharacter.h"
#include "GameFramework/PlayerController.h"

AT66SimpleGameMode::AT66SimpleGameMode()
{
	// Set default pawn class to our simple character
	DefaultPawnClass = AT66SimpleCharacter::StaticClass();
	
	// Use default player controller
	PlayerControllerClass = APlayerController::StaticClass();
}
