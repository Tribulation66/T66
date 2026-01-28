#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "T66WhiteModeSubsystem.generated.h"

/**
 * Owns White Mode (event state). White items are true damage sources.
 * While active: other item procs should be blocked (wired later).
 */
UCLASS()
class T66_API UT66WhiteModeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="T66|White")
	bool IsWhiteModeActive() const { return bWhiteModeActive; }

	UFUNCTION(BlueprintCallable, Category="T66|White")
	void StartWhiteModeById(FName WhiteItemId);

	UFUNCTION(BlueprintCallable, Category="T66|White")
	void StopWhiteMode();

private:
	UPROPERTY(Transient)
	bool bWhiteModeActive = false;
};
