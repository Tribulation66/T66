#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "T66HeroData.h"
#include "T66SelectionSubsystem.generated.h"

/**
 * Manages hero and companion selection state
 */
UCLASS()
class T66_API UT66SelectionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Heroes
	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	const TArray<FT66HeroData>& GetAllHeroes() const { return AvailableHeroes; }

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	int32 GetHeroCount() const { return AvailableHeroes.Num(); }

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	const FT66HeroData& GetHeroAt(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	void SelectHero(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	int32 GetSelectedHeroIndex() const { return SelectedHeroIndex; }

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	const FT66HeroData& GetSelectedHero() const;

	// Companions
	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	const TArray<FT66CompanionData>& GetAllCompanions() const { return AvailableCompanions; }

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	int32 GetCompanionCount() const { return AvailableCompanions.Num(); }

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	const FT66CompanionData& GetCompanionAt(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	void SelectCompanion(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	int32 GetSelectedCompanionIndex() const { return SelectedCompanionIndex; }

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	const FT66CompanionData& GetSelectedCompanion() const;

	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	bool HasCompanionSelected() const { return SelectedCompanionIndex >= 0; }

	// No companion option
	UFUNCTION(BlueprintCallable, Category = "T66|Selection")
	void SelectNoCompanion() { SelectedCompanionIndex = -1; }

private:
	void InitializeHeroes();
	void InitializeCompanions();

	UPROPERTY()
	TArray<FT66HeroData> AvailableHeroes;

	UPROPERTY()
	TArray<FT66CompanionData> AvailableCompanions;

	int32 SelectedHeroIndex = 0;
	int32 SelectedCompanionIndex = 0; // -1 means no companion
};
