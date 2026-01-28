#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66WhiteItemRow.generated.h"

UENUM(BlueprintType)
enum class ET66WhitePatternKind : uint8
{
	None UMETA(DisplayName="None"),
	MultiSlash UMETA(DisplayName="Multi Slash"),
	AreaPulse UMETA(DisplayName="Area Pulse"),
	Storm UMETA(DisplayName="Storm"),
	Custom UMETA(DisplayName="Custom")
};

/**
 * DT_WhiteItems row.
 * Canonical ID = RowName.
 *
 * White items are TRUE damage sources:
 * - One-shot mobs (guaranteed overkill)
 * - Deal fixed total % HP to bosses over the white duration (split over hits)
 */
USTRUCT(BlueprintType)
struct T66_API FT66WhiteItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Type")
	ET66WhitePatternKind PatternKind = ET66WhitePatternKind::None;

	// White mode timing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|White", meta=(ClampMin="0.0"))
	float DurationSeconds = 6.0f;

	// Boss rule: total percent HP dealt over DurationSeconds
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|White", meta=(ClampMin="0.0", ClampMax="1.0"))
	float BossTotalPercentHP = 0.25f;

	// Boss rule: number of discrete hits used to split BossTotalPercentHP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|White", meta=(ClampMin="1"))
	int32 BossHitCount = 10;

	// Mob rule: ensure overkill feel; white guarantees one-shot anyway
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|White", meta=(ClampMin="1.0"))
	float MobOverkillMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
