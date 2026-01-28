#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "T66EconomySubsystem.generated.h"

class AController;
class APlayerState;
class UT66WalletComponent;
struct FT66GoldTxnContext;

UCLASS()
class T66_API UT66EconomySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="T66|Economy")
	UT66WalletComponent* FindWalletFromController(AController* Controller) const;

	UFUNCTION(BlueprintCallable, Category="T66|Economy")
	UT66WalletComponent* FindWalletFromPlayerState(APlayerState* PlayerState) const;

	UFUNCTION(BlueprintCallable, Category="T66|Economy")
	void AddGold(AController* Controller, int64 Amount, const FT66GoldTxnContext& Context);

	UFUNCTION(BlueprintCallable, Category="T66|Economy")
	bool TrySpendGold(AController* Controller, int64 Amount, const FT66GoldTxnContext& Context);
};
