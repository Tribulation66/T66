#include "T66SelectionSubsystem.h"

void UT66SelectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	InitializeHeroes();
	InitializeCompanions();
	
	UE_LOG(LogTemp, Display, TEXT("[T66SelectionSubsystem] Initialized with %d heroes and %d companions"), 
		AvailableHeroes.Num(), AvailableCompanions.Num());
}

void UT66SelectionSubsystem::InitializeHeroes()
{
	// Create 6 heroes with distinct colors
	AvailableHeroes.Empty();
	
	AvailableHeroes.Add(FT66HeroData(
		FName(TEXT("Hero_Warrior")),
		TEXT("Warrior"),
		TEXT("A fierce melee combatant with high defense."),
		FLinearColor(0.8f, 0.2f, 0.2f, 1.0f) // Red
	));
	
	AvailableHeroes.Add(FT66HeroData(
		FName(TEXT("Hero_Mage")),
		TEXT("Mage"),
		TEXT("A powerful spellcaster with devastating magic."),
		FLinearColor(0.2f, 0.2f, 0.9f, 1.0f) // Blue
	));
	
	AvailableHeroes.Add(FT66HeroData(
		FName(TEXT("Hero_Rogue")),
		TEXT("Rogue"),
		TEXT("A swift assassin who strikes from the shadows."),
		FLinearColor(0.4f, 0.1f, 0.6f, 1.0f) // Purple
	));
	
	AvailableHeroes.Add(FT66HeroData(
		FName(TEXT("Hero_Ranger")),
		TEXT("Ranger"),
		TEXT("A skilled archer with deadly precision."),
		FLinearColor(0.2f, 0.7f, 0.2f, 1.0f) // Green
	));
	
	AvailableHeroes.Add(FT66HeroData(
		FName(TEXT("Hero_Paladin")),
		TEXT("Paladin"),
		TEXT("A holy knight with healing abilities."),
		FLinearColor(0.9f, 0.85f, 0.2f, 1.0f) // Gold
	));
	
	AvailableHeroes.Add(FT66HeroData(
		FName(TEXT("Hero_Berserker")),
		TEXT("Berserker"),
		TEXT("A raging warrior who grows stronger in combat."),
		FLinearColor(0.9f, 0.5f, 0.1f, 1.0f) // Orange
	));
}

void UT66SelectionSubsystem::InitializeCompanions()
{
	// Create 4 companions with distinct colors
	AvailableCompanions.Empty();
	
	AvailableCompanions.Add(FT66CompanionData(
		FName(TEXT("Companion_Sprite")),
		TEXT("Lumina"),
		TEXT("She tells me... of a time when the light was all there was."),
		FLinearColor(0.9f, 0.9f, 0.5f, 1.0f) // Light Yellow
	));
	
	AvailableCompanions.Add(FT66CompanionData(
		FName(TEXT("Companion_Shadow")),
		TEXT("Umbra"),
		TEXT("She tells me... secrets whispered in the darkness."),
		FLinearColor(0.3f, 0.3f, 0.4f, 1.0f) // Dark Gray
	));
	
	AvailableCompanions.Add(FT66CompanionData(
		FName(TEXT("Companion_Flame")),
		TEXT("Ember"),
		TEXT("She tells me... of fires that never die."),
		FLinearColor(1.0f, 0.4f, 0.1f, 1.0f) // Orange-Red
	));
	
	AvailableCompanions.Add(FT66CompanionData(
		FName(TEXT("Companion_Frost")),
		TEXT("Crystal"),
		TEXT("She tells me... of frozen dreams beneath the ice."),
		FLinearColor(0.5f, 0.8f, 1.0f, 1.0f) // Ice Blue
	));
}

const FT66HeroData& UT66SelectionSubsystem::GetHeroAt(int32 Index) const
{
	static FT66HeroData EmptyHero;
	if (AvailableHeroes.IsValidIndex(Index))
	{
		return AvailableHeroes[Index];
	}
	return EmptyHero;
}

void UT66SelectionSubsystem::SelectHero(int32 Index)
{
	if (AvailableHeroes.IsValidIndex(Index))
	{
		SelectedHeroIndex = Index;
		UE_LOG(LogTemp, Display, TEXT("[T66SelectionSubsystem] Selected hero: %s"), *AvailableHeroes[Index].DisplayName);
	}
}

const FT66HeroData& UT66SelectionSubsystem::GetSelectedHero() const
{
	return GetHeroAt(SelectedHeroIndex);
}

const FT66CompanionData& UT66SelectionSubsystem::GetCompanionAt(int32 Index) const
{
	static FT66CompanionData EmptyCompanion;
	if (AvailableCompanions.IsValidIndex(Index))
	{
		return AvailableCompanions[Index];
	}
	return EmptyCompanion;
}

void UT66SelectionSubsystem::SelectCompanion(int32 Index)
{
	if (Index < 0)
	{
		SelectedCompanionIndex = -1;
		UE_LOG(LogTemp, Display, TEXT("[T66SelectionSubsystem] Selected: No Companion"));
	}
	else if (AvailableCompanions.IsValidIndex(Index))
	{
		SelectedCompanionIndex = Index;
		UE_LOG(LogTemp, Display, TEXT("[T66SelectionSubsystem] Selected companion: %s"), *AvailableCompanions[Index].DisplayName);
	}
}

const FT66CompanionData& UT66SelectionSubsystem::GetSelectedCompanion() const
{
	return GetCompanionAt(SelectedCompanionIndex);
}
