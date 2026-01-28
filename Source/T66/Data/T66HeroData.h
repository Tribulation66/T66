#pragma once

#include "CoreMinimal.h"
#include "T66HeroData.generated.h"

/**
 * Represents a hero in the game (displayed as a colored cylinder)
 */
USTRUCT(BlueprintType)
struct T66_API FT66HeroData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HeroID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor BodyColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath IconTexture;

	FT66HeroData()
		: HeroID(NAME_None)
		, DisplayName(TEXT("Unknown"))
		, Description(TEXT(""))
		, BodyColor(FLinearColor::White)
	{}

	FT66HeroData(FName InID, const FString& InName, const FString& InDesc, FLinearColor InColor)
		: HeroID(InID)
		, DisplayName(InName)
		, Description(InDesc)
		, BodyColor(InColor)
	{}
};

/**
 * Represents a companion in the game (displayed as a colored cube)
 */
USTRUCT(BlueprintType)
struct T66_API FT66CompanionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CompanionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor BodyColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PassiveEffect;

	FT66CompanionData()
		: CompanionID(NAME_None)
		, DisplayName(TEXT("Unknown"))
		, Description(TEXT(""))
		, BodyColor(FLinearColor::White)
		, PassiveEffect(TEXT("Heals the hero over time"))
	{}

	FT66CompanionData(FName InID, const FString& InName, const FString& InDesc, FLinearColor InColor)
		: CompanionID(InID)
		, DisplayName(InName)
		, Description(InDesc)
		, BodyColor(InColor)
		, PassiveEffect(TEXT("Heals the hero over time"))
	{}
};
