#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "T66BossRow.generated.h"

/**
 * Row struct for DT_Bosses (DT-first pipeline).
 * Keep this focused on "data" (rules + identifiers). Presentation assets can be referenced softly.
 */
USTRUCT(BlueprintType)
struct T66_API FT66BossRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Canonical ID tag for this boss (ex: ID.Boss.<Name>)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ID")
	FGameplayTag BossId;

	// Display label (UI)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Text")
	FText DisplayName;

	// Optional: boss actor/pawn class to spawn (presentation/implementation hook)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn")
	TSoftClassPtr<AActor> BossClass;

	// Cross-reference to attack patterns (if you want bosses to pick patterns by ID)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TArray<FGameplayTag> AttackPatternIds;

	// Minimal numeric hooks (tunable later; can be replaced/expanded as we implement boss systems)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float BaseHP = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float ContactDamage = 0.0f;
};
