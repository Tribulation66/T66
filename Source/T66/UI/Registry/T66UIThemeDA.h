#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "T66UIThemeDA.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FT66UIThemeTextStyle
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme")
	int32 FontSize = 18;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme")
	float LetterSpacing = 0.0f;
};

UCLASS(BlueprintType)
class T66_API UT66UIThemeDA : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ----------------------------
	// Existing basic tokens
	// ----------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme")
	FT66UIThemeTextStyle BodyText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme")
	FT66UIThemeTextStyle TitleText;

	// ----------------------------
	// Expandable Theme Token Maps
	// (lets us auto-fill DA_UITheme_* assets)
	// ----------------------------

	// Colors by token name (ex: "Text.Primary", "Panel.Bg", "Accent.1")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme|Tokens")
	TMap<FName, FLinearColor> ColorTokens;

	// Numbers by token name (ex: "Text.Size.M", "Opacity.Disabled", "Anim.Speed")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme|Tokens")
	TMap<FName, float> FloatTokens;

	// Margins/padding by token name (ex: "Padding.Button", "Padding.Panel")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme|Tokens")
	TMap<FName, FMargin> MarginTokens;

	// 2D sizes by token name (ex: "Icon.Size.S", "Button.MinSize")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme|Tokens")
	TMap<FName, FVector2D> Vector2Tokens;

	// Audio references by token name (ex: "SFX.UI.Hover", "SFX.UI.Click")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme|Tokens")
	TMap<FName, TObjectPtr<USoundBase>> SoundTokens;
};
