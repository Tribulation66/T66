#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "T66UIShellSubsystem.generated.h"

class UUserWidget;
class UButton;

/**
 * UT66UIShellSubsystem
 * - Auto-spawns Main Menu when PIE starts
 * - Handles all screen navigation
 * - Binds button clicks to screen transitions
 */
UCLASS()
class T66_API UT66UIShellSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Navigate to a screen by widget path
	UFUNCTION(BlueprintCallable, Category = "T66|UI")
	void NavigateToScreen(const FString& ScreenPath);

	// Close current screen and return to previous
	UFUNCTION(BlueprintCallable, Category = "T66|UI")
	void NavigateBack();

	// Start the game (enter gameplay map)
	UFUNCTION(BlueprintCallable, Category = "T66|UI")
	void StartGameplay();

private:
	// Called when world initializes
	void HandleWorldInit(UWorld* World, const UWorld::InitializationValues IVS);

	// Deferred spawn after viewport is ready
	void SpawnMainMenuDeferred();

	// Spawns Main Menu screen
	void SpawnMainMenu(UWorld* World);

	// Binds all button click handlers
	void BindMainMenuButtons();

	// Find a button in the spawned main menu by partial name
	UButton* FindButtonInMenu(const FString& ButtonNameContains);

	// Navigation handlers
	UFUNCTION() void OnNewGameClicked();
	UFUNCTION() void OnLoadGameClicked();
	UFUNCTION() void OnSettingsClicked();
	UFUNCTION() void OnAchievementsClicked();
	UFUNCTION() void OnAccountStatusClicked();
	UFUNCTION() void OnLanguageClicked();
	UFUNCTION() void OnQuitClicked();

	// Party size selection
	UFUNCTION() void OnSoloClicked();
	UFUNCTION() void OnDuoClicked();
	UFUNCTION() void OnTrioClicked();

	// Back navigation
	UFUNCTION() void OnBackClicked();
	void BindBackButton();

	// Hero selection - individual handlers for each hero
	UFUNCTION() void OnHero0Clicked();
	UFUNCTION() void OnHero1Clicked();
	UFUNCTION() void OnHero2Clicked();
	UFUNCTION() void OnHero3Clicked();
	UFUNCTION() void OnHero4Clicked();
	UFUNCTION() void OnHero5Clicked();
	UFUNCTION() void OnPrevHeroClicked();
	UFUNCTION() void OnNextHeroClicked();
	UFUNCTION() void OnChooseCompanionClicked();
	UFUNCTION() void OnEnterTribulationClicked();
	void BindHeroSelectionButtons();
	void SelectHero(int32 Index);
	void UpdateHeroDisplay();

	// Companion selection - individual handlers
	UFUNCTION() void OnCompanion0Clicked();
	UFUNCTION() void OnCompanion1Clicked();
	UFUNCTION() void OnCompanion2Clicked();
	UFUNCTION() void OnCompanion3Clicked();
	UFUNCTION() void OnPrevCompanionClicked();
	UFUNCTION() void OnNextCompanionClicked();
	UFUNCTION() void OnNoCompanionClicked();
	UFUNCTION() void OnConfirmCompanionClicked();
	void BindCompanionSelectionButtons();
	void SelectCompanion(int32 Index);
	void UpdateCompanionDisplay();

	// Helper to spawn a screen widget
	UUserWidget* SpawnScreen(const FString& ScreenPath, int32 ZOrder = 100);

	// Helper to close current screen
	void CloseCurrentScreen();

private:
	// Tracks current screen widget
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> CurrentScreen = nullptr;

	// Stack of screens for back navigation
	UPROPERTY(Transient)
	TArray<TObjectPtr<UUserWidget>> ScreenStack;

	// Delegate handle
	FDelegateHandle OnWorldInitHandle;

	// Current world reference
	TWeakObjectPtr<UWorld> CurrentWorld;

	// Deferred binding timer
	FTimerHandle ButtonBindTimer;

	// Flag to track if we're entering gameplay (don't spawn menu)
	bool bEnteringGameplay = false;
};
