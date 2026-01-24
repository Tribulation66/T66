#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "T66UIShellSubsystem.generated.h"

class UUserWidget;

/**
 * UT66UIShellSubsystem
 * - Auto-spawns WBP_UIRoot at runtime (PIE/Game)
 * - No manual level wiring required
 */
UCLASS()
class T66_API UT66UIShellSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	// Called after a map finishes loading
	void HandlePostLoadMapWithWorld(UWorld* LoadedWorld);

	// Actually creates + adds UIRoot to viewport
	void SpawnUIRoot(UWorld* World);

private:
	// Tracks our spawned root widget (so we only spawn once)
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> SpawnedUIRoot = nullptr;

	// Delegate handle so we can cleanly unhook
	FDelegateHandle PostLoadMapHandle;
};
