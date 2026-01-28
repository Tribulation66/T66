#include "T66EconomySubsystem.h"

#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"

#include "T66/T66PlayerState.h"
#include "T66/Economy/Wallet/T66WalletComponent.h"

UT66WalletComponent* UT66EconomySubsystem::FindWalletFromController(AController* Controller) const
{
	return Controller ? FindWalletFromPlayerState(Controller->PlayerState) : nullptr;
}

UT66WalletComponent* UT66EconomySubsystem::FindWalletFromPlayerState(APlayerState* PlayerState) const
{
	if (AT66PlayerState* PS = Cast<AT66PlayerState>(PlayerState))
	{
		return PS->GetWallet();
	}
	return nullptr;
}

void UT66EconomySubsystem::AddGold(AController* Controller, int64 Amount, const FT66GoldTxnContext& Context)
{
	UT66WalletComponent* Wallet = FindWalletFromController(Controller);
	if (!Wallet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66Economy] AddGold failed: no wallet for controller=%s"), *GetNameSafe(Controller));
		return;
	}

	Wallet->AddGold(Amount, Context);
}

bool UT66EconomySubsystem::TrySpendGold(AController* Controller, int64 Amount, const FT66GoldTxnContext& Context)
{
	UT66WalletComponent* Wallet = FindWalletFromController(Controller);
	if (!Wallet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66Economy] TrySpendGold failed: no wallet for controller=%s"), *GetNameSafe(Controller));
		return false;
	}

	return Wallet->TrySpendGold(Amount, Context);
}
