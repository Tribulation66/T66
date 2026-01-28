#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66UltimateRow.generated.h"

UENUM(BlueprintType)
enum class ET66UltimateEffectKind : uint8
{
	None UMETA(DisplayName="None"),
	DashBurst UMETA(DisplayName="Dash Burst"),
	MultiSlash UMETA(DisplayName="Multi Slash"),
	TimeStop UMETA(DisplayName="Time Stop"),
	Storm UMETA(DisplayName="Storm"),
	Custom UMETA(DisplayName="Custom")
};

/**
 * DT_Ultimates row.
 * Canonical ID = RowName.
 */
USTRUCT(BlueprintType)
struct T66_API FT66UltimateRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Type")
	ET66UltimateEffectKind EffectKind = ET66UltimateEffectKind::None;

	// Generic tuning (used by many ultimates; specific runners can interpret)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Tuning", meta=(ClampMin="0.0"))
	float DurationSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Tuning", meta=(ClampMin="0.0"))
	float CooldownSeconds = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Tuning", meta=(ClampMin="0.0"))
	float Radius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
