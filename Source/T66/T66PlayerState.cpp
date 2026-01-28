#include "T66PlayerState.h"

#include "T66/Economy/Wallet/T66WalletComponent.h"

AT66PlayerState::AT66PlayerState()
{
	Wallet = CreateDefaultSubobject<UT66WalletComponent>(TEXT("Wallet"));
}
