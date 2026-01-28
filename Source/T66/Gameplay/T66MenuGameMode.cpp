#include "T66MenuGameMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpectatorPawn.h"

AT66MenuGameMode::AT66MenuGameMode()
{
	// Don't spawn a player pawn for the menu - use spectator instead
	DefaultPawnClass = ASpectatorPawn::StaticClass();
	PlayerControllerClass = APlayerController::StaticClass();
	
	// Hide the cursor and focus on UI
	bStartPlayersAsSpectators = true;
}
