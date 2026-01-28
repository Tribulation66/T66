#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "T66/Data/Reg/T66DataCatalogDA.h"
#include "T66/Data/Schema/T66ItemRow.h"
#include "T66/Data/Schema/T66EnemyRow.h"
#include "T66/Data/Schema/T66BossRow.h"
#include "T66/Data/Schema/T66StageEffectRow.h"
#include "T66/Data/Schema/T66CompanionRow.h"
#include "T66/Data/Schema/T66AttackPatternRow.h"
#include "T66/Data/Schema/T66MovementProfileRow.h"
#include "T66/Data/Schema/T66LuckSourceRow.h"
#include "T66/Data/Schema/Loot/T66LootBagDropRow.h"
#include "T66/Data/Schema/Loot/T66LootBagRarityRow.h"
#include "T66/Data/Schema/Loot/T66LootPoolRow.h"
#include "T66/Data/Schema/Gambling/T66GambleOutcomeRow.h"
#include "T66/Data/Schema/Economy/T66LoanSharkRulesRow.h"
#include "T66/Data/Schema/Stages/T66StageRow.h"
#include "T66/Data/Schema/Hero/T66HeroRow.h"
#include "T66/Data/Schema/Idols/T66IdolRow.h"
#include "T66/Data/Schema/Achievements/T66AchievementRow.h"
#include "T66/Data/Schema/Director/T66SpawnWeightingRow.h"
#include "T66DataCatalogSubsystem.generated.h"

/**
 * Runtime access point for canonical DTs (DT-first pipeline).
 * RowName == ID.
 */
UCLASS(Config=Game)
class T66_API UT66DataCatalogSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(Config)
	FSoftObjectPath DataCatalogAssetPath;

	UFUNCTION(BlueprintPure, Category="T66|Data")
	UT66DataCatalogDA* GetCatalog();

	bool GetItemRow(FName RowId, FT66ItemRow& OutRow);
	bool GetEnemyRow(FName RowId, FT66EnemyRow& OutRow);
	bool GetBossRow(FName RowId, FT66BossRow& OutRow);
	bool GetStageEffectRow(FName RowId, FT66StageEffectRow& OutRow);
	bool GetCompanionRow(FName RowId, FT66CompanionRow& OutRow);

	bool GetAttackPatternRow(FName RowId, FT66AttackPatternRow& OutRow);
	bool GetMovementProfileRow(FName RowId, FT66MovementProfileRow& OutRow);
	bool GetLuckSourceRow(FName RowId, FT66LuckSourceRow& OutRow);

	bool GetLootBagDropRow(FName RowId, FT66LootBagDropRow& OutRow);
	bool GetLootBagRarityRow(FName RowId, FT66LootBagRarityRow& OutRow);
	bool GetLootPoolRow(FName RowId, FT66LootPoolRow& OutRow);
	bool GetGambleOutcomeRow(FName RowId, FT66GambleOutcomeRow& OutRow);
	bool GetLoanSharkRulesRow(FName RowId, FT66LoanSharkRulesRow& OutRow);
	bool GetStageRow(FName RowId, FT66StageRow& OutRow);
	bool GetHeroRow(FName RowId, FT66HeroRow& OutRow);
	bool GetIdolRow(FName RowId, FT66IdolRow& OutRow);
	bool GetAchievementRow(FName RowId, FT66AchievementRow& OutRow);
	bool GetSpawnWeightingRow(FName RowId, FT66SpawnWeightingRow& OutRow);

private:
	UPROPERTY(Transient)
	TObjectPtr<UT66DataCatalogDA> LoadedCatalog;

	UDataTable* LoadTable(const TSoftObjectPtr<UDataTable>& TablePtr) const;

	template<typename TRow>
	bool GetRow(UDataTable* Table, FName RowId, TRow& OutRow) const
	{
		if (!Table || RowId.IsNone())
		{
			return false;
		}

		static const FString Context(TEXT("T66DataCatalogSubsystem"));
		const TRow* Found = Table->FindRow<TRow>(RowId, Context, /*bWarnIfRowMissing*/ false);
		if (!Found)
		{
			return false;
		}

		OutRow = *Found;
		return true;
	}
};
