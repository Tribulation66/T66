#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "T66DataCatalogDA.generated.h"

class UDataTable;


/**
 * Canonical references to DataTables (DT-first pipeline).
 * Keeps runtime/tools from hardcoding /Game paths.
 */
UCLASS(BlueprintType)
class T66_API UT66DataCatalogDA : public UDataAsset
{
	GENERATED_BODY()

public:
	// Core bulk content
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Catalog")
	TSoftObjectPtr<UDataTable> ItemsTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Catalog")
	TSoftObjectPtr<UDataTable> EnemiesTable;

	// NEW: bosses are distinct from general enemies
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Catalog")
	TSoftObjectPtr<UDataTable> BossesTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Catalog")
	TSoftObjectPtr<UDataTable> StageEffectsTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Catalog")
	TSoftObjectPtr<UDataTable> CompanionsTable;

	// Supporting DTs (enemy cross-references + luck model)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Catalog")
	TSoftObjectPtr<UDataTable> AttackPatternsTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Catalog")
	TSoftObjectPtr<UDataTable> MovementProfilesTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Catalog")
	TSoftObjectPtr<UDataTable> LuckSourcesTable;

	// Combat / special behavior DTs
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Catalog")
	TSoftObjectPtr<UDataTable> UltimatesTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="T66|Catalog")
	TSoftObjectPtr<UDataTable> WhiteItemsTable;

	// Loot / drops
	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Loot")
	TSoftObjectPtr<UDataTable> LootBagDropsTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Loot")
	TSoftObjectPtr<UDataTable> LootBagRarityTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Loot")
	TSoftObjectPtr<UDataTable> LootPoolTable;


	// NPC / Interactables data tables

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|NPC")
	TSoftObjectPtr<UDataTable> NPCProfilesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|NPC")
	TSoftObjectPtr<UDataTable> VendorRulesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|NPC")
	TSoftObjectPtr<UDataTable> GamblerRulesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|NPC")
	TSoftObjectPtr<UDataTable> SaintRulesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|NPC")
	TSoftObjectPtr<UDataTable> TricksterRulesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|NPC")
	TSoftObjectPtr<UDataTable> OuroborosRulesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|NPC")
	TSoftObjectPtr<UDataTable> StealMatrixTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|NPC")
	TSoftObjectPtr<UDataTable> CheatMatrixTable;

	// Gambling + debt
	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Gambling")
	TSoftObjectPtr<UDataTable> GambleOutcomesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Economy")
	TSoftObjectPtr<UDataTable> LoanSharkRulesTable;

	// Stages + heroes
	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Stages")
	TSoftObjectPtr<UDataTable> StagesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Heroes")
	TSoftObjectPtr<UDataTable> HeroesTable;


	// World Interactables + Special Enemies DTs

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|World")
	TSoftObjectPtr<UDataTable> InteractablesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|World|Interactables")
	TSoftObjectPtr<UDataTable> SpinWheelRulesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|World|Interactables")
	TSoftObjectPtr<UDataTable> CashTruckRulesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|World|Interactables")
	TSoftObjectPtr<UDataTable> LifeTreeRulesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Enemy|Special")
	TSoftObjectPtr<UDataTable> SpecialEnemyProfilesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Enemy|Special")
	TSoftObjectPtr<UDataTable> GoblinThiefRulesTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Enemy|Special")
	TSoftObjectPtr<UDataTable> LeprechaunRulesTable;

	// Meta / progression
	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Meta")
	TSoftObjectPtr<UDataTable> IdolsTable;

	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Meta")
	TSoftObjectPtr<UDataTable> AchievementsTable;

	// Spawn director
	UPROPERTY(EditDefaultsOnly, Category="T66|Catalog|Director")
	TSoftObjectPtr<UDataTable> SpawnWeightingTable;

};
