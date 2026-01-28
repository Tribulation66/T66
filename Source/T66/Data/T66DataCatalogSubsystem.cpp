#include "T66/Data/T66DataCatalogSubsystem.h"

UT66DataCatalogDA* UT66DataCatalogSubsystem::GetCatalog()
{
	if (LoadedCatalog)
	{
		return LoadedCatalog;
	}

	if (!DataCatalogAssetPath.IsValid())
	{
		return nullptr;
	}

	UObject* Obj = DataCatalogAssetPath.TryLoad();
	LoadedCatalog = Cast<UT66DataCatalogDA>(Obj);
	return LoadedCatalog;
}

UDataTable* UT66DataCatalogSubsystem::LoadTable(const TSoftObjectPtr<UDataTable>& TablePtr) const
{
	return TablePtr.IsNull() ? nullptr : TablePtr.LoadSynchronous();
}

bool UT66DataCatalogSubsystem::GetItemRow(FName RowId, FT66ItemRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66ItemRow>(LoadTable(Catalog->ItemsTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetEnemyRow(FName RowId, FT66EnemyRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66EnemyRow>(LoadTable(Catalog->EnemiesTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetBossRow(FName RowId, FT66BossRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66BossRow>(LoadTable(Catalog->BossesTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetStageEffectRow(FName RowId, FT66StageEffectRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66StageEffectRow>(LoadTable(Catalog->StageEffectsTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetCompanionRow(FName RowId, FT66CompanionRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66CompanionRow>(LoadTable(Catalog->CompanionsTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetAttackPatternRow(FName RowId, FT66AttackPatternRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66AttackPatternRow>(LoadTable(Catalog->AttackPatternsTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetMovementProfileRow(FName RowId, FT66MovementProfileRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66MovementProfileRow>(LoadTable(Catalog->MovementProfilesTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetLuckSourceRow(FName RowId, FT66LuckSourceRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66LuckSourceRow>(LoadTable(Catalog->LuckSourcesTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetLootBagDropRow(FName RowId, FT66LootBagDropRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66LootBagDropRow>(LoadTable(Catalog->LootBagDropsTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetLootBagRarityRow(FName RowId, FT66LootBagRarityRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66LootBagRarityRow>(LoadTable(Catalog->LootBagRarityTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetLootPoolRow(FName RowId, FT66LootPoolRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66LootPoolRow>(LoadTable(Catalog->LootPoolTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetGambleOutcomeRow(FName RowId, FT66GambleOutcomeRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66GambleOutcomeRow>(LoadTable(Catalog->GambleOutcomesTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetLoanSharkRulesRow(FName RowId, FT66LoanSharkRulesRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66LoanSharkRulesRow>(LoadTable(Catalog->LoanSharkRulesTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetStageRow(FName RowId, FT66StageRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66StageRow>(LoadTable(Catalog->StagesTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetHeroRow(FName RowId, FT66HeroRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66HeroRow>(LoadTable(Catalog->HeroesTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetIdolRow(FName RowId, FT66IdolRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66IdolRow>(LoadTable(Catalog->IdolsTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetAchievementRow(FName RowId, FT66AchievementRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66AchievementRow>(LoadTable(Catalog->AchievementsTable), RowId, OutRow);
}

bool UT66DataCatalogSubsystem::GetSpawnWeightingRow(FName RowId, FT66SpawnWeightingRow& OutRow)
{
	UT66DataCatalogDA* Catalog = GetCatalog();
	if (!Catalog) return false;
	return GetRow<FT66SpawnWeightingRow>(LoadTable(Catalog->SpawnWeightingTable), RowId, OutRow);
}
