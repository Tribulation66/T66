#include "T66WalletComponent.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UT66WalletComponent::UT66WalletComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UT66WalletComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("[T66Wallet] BeginPlay Owner=%s Gold=%lld"),
		*GetNameSafe(GetOwner()),
		(long long)CurrentGold);
}

void UT66WalletComponent::OnRep_CurrentGold()
{
	OnGoldChanged.Broadcast(CurrentGold);
}

void UT66WalletComponent::AddGold(int64 Amount, const FT66GoldTxnContext& Context)
{
	if (Amount <= 0)
	{
		return;
	}

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66Wallet] AddGold called without authority. Owner=%s"), *GetNameSafe(GetOwner()));
		return;
	}

	ApplyDelta(Amount, Context);
}

bool UT66WalletComponent::TrySpendGold(int64 Amount, const FT66GoldTxnContext& Context)
{
	if (Amount <= 0)
	{
		return true;
	}

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66Wallet] TrySpendGold called without authority. Owner=%s"), *GetNameSafe(GetOwner()));
		return false;
	}

	if (CurrentGold < Amount)
	{
		return false;
	}

	ApplyDelta(-Amount, Context);
	return true;
}

void UT66WalletComponent::ApplyDelta(int64 Delta, const FT66GoldTxnContext& Context)
{
	const int64 Before = CurrentGold;
	CurrentGold = FMath::Max<int64>(0, CurrentGold + Delta);

	FT66GoldTxn Txn;
	Txn.Delta = Delta;
	Txn.After = CurrentGold;
	Txn.Context = Context;
	Txn.TimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	Ledger.Add(Txn);

	UE_LOG(LogTemp, Display, TEXT("[T66Wallet] Delta=%lld Before=%lld After=%lld Source=%s Reason=%s Stage=%d"),
		(long long)Delta,
		(long long)Before,
		(long long)CurrentGold,
		*Context.SourceId.ToString(),
		*Context.ReasonId.ToString(),
		Context.StageIndex);

	OnGoldChanged.Broadcast(CurrentGold);

	// Mark owner so replication updates promptly.
	// (PlayerState replicates; this helps push updates.)
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
}

void UT66WalletComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UT66WalletComponent, CurrentGold);
}
