#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "T66UIThemeDA.generated.h"

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
	// Basic tokens (we can expand later without breaking anything)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme")
	FT66UIThemeTextStyle BodyText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI Theme")
	FT66UIThemeTextStyle TitleText;
};
