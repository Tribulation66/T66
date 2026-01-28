#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "T66WalletComponent.generated.h"

USTRUCT(BlueprintType)
struct FT66GoldTxnContext
{
	GENERATED_BODY()

	// Optional identifiers for later analytics / anti-cheat provenance
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SourceId = NAME_None;   // e.g. Interactable.CashTruck

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReasonId = NAME_None;   // e.g. GoldPayout / VendorPurchase / LootBag

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StageIndex = -1;
};

USTRUCT()
struct FT66GoldTxn
{
	GENERATED_BODY()

	UPROPERTY()
	int64 Delta = 0;

	UPROPERTY()
	int64 After = 0;

	UPROPERTY()
	FT66GoldTxnContext Context;

	UPROPERTY()
	double TimeSeconds = 0.0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FT66OnGoldChanged, int64, NewGold);

UCLASS(ClassGroup=(T66), meta=(BlueprintSpawnableComponent))
class T66_API UT66WalletComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UT66WalletComponent();

	// Authoritative gold. Replicated so UI can show it.
	UPROPERTY(ReplicatedUsing=OnRep_CurrentGold, VisibleAnywhere, BlueprintReadOnly, Category="Wallet")
	int64 CurrentGold = 0;

	UPROPERTY(BlueprintAssignable, Category="Wallet")
	FT66OnGoldChanged OnGoldChanged;

	UFUNCTION(BlueprintCallable, Category="Wallet")
	int64 GetGold() const { return CurrentGold; }

	// Server-authoritative operations (call from server gameplay code).
	UFUNCTION(BlueprintCallable, Category="Wallet")
	void AddGold(int64 Amount, const FT66GoldTxnContext& Context);

	UFUNCTION(BlueprintCallable, Category="Wallet")
	bool TrySpendGold(int64 Amount, const FT66GoldTxnContext& Context);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_CurrentGold();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void ApplyDelta(int64 Delta, const FT66GoldTxnContext& Context);

	// Server-only ledger (not replicated). Useful later for anti-cheat validation.
	UPROPERTY()
	TArray<FT66GoldTxn> Ledger;
};
