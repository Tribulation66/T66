#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "T66PlayerState.generated.h"

class UT66WalletComponent;

UCLASS()
class T66_API AT66PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AT66PlayerState();

	UFUNCTION(BlueprintCallable, Category="T66|Economy")
	UT66WalletComponent* GetWallet() const { return Wallet; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="T66|Economy")
	TObjectPtr<UT66WalletComponent> Wallet;
};
