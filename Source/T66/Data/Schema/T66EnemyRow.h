#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66EnemyRow.generated.h"

/**
 * DT_Enemies row.
 * Canonical ID = the DataTable Row Name.
 * Cross references also use row IDs (FName).
 */
USTRUCT(BlueprintType)
struct T66_API FT66EnemyRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Text")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Enemy", meta=(ClampMin="0"))
	int32 PointValue = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Enemy|Movement", meta=(ClampMin="0.0"))
	float MoveSpeed = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Enemy|Damage", meta=(ClampMin="0.0"))
	float BaseDamage = 1.0f;

	/** e.g. "BurrowTeleportFollower", "Strafe", "Charge" (row ID in a future DT_MovementProfiles) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Enemy|Movement")
	FName MovementProfileId;

	/** e.g. "SpitTriple", "SwipeCombo", "DashRam" (row ID in a future DT_AttackPatterns) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Enemy|Attack")
	FName AttackPatternId;

	/** Optional: identifies this enemy as a luck model source (row ID in future DT_LuckSources) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Enemy|Luck")
	FName LuckSourceId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Flags")
	bool bEnabled = true;
};
