#include "T66CoreDataTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorAssetLibrary.h"
#include "Engine/DataTable.h"
#include "UObject/Package.h"

#include "T66/Data/Reg/T66DataCatalogDA.h"
#include "T66/Data/Schema/T66ItemRow.h"
#include "T66/Data/Schema/T66EnemyRow.h"
#include "T66/Data/Schema/T66BossRow.h"
#include "T66/Data/Schema/T66StageEffectRow.h"
#include "T66/Data/Schema/T66CompanionRow.h"
#include "T66/Data/Schema/T66AttackPatternRow.h"
#include "T66/Data/Schema/T66MovementProfileRow.h"
#include "T66/Data/Schema/T66LuckSourceRow.h"
#include "T66/Data/Schema/T66UltimateRow.h"
#include "T66/Data/Schema/T66WhiteItemRow.h"
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

static UDataTable* EnsureDataTable(const FString& FolderPath, const FString& AssetName, UScriptStruct* RowStruct)
{
	UEditorAssetLibrary::MakeDirectory(FolderPath);

	const FString PackagePath = FolderPath + TEXT("/") + AssetName;
	const FString ObjectPath  = PackagePath + TEXT(".") + AssetName;

	if (UEditorAssetLibrary::DoesAssetExist(ObjectPath))
	{
		UObject* Loaded = UEditorAssetLibrary::LoadAsset(ObjectPath);
		UDataTable* DT = Cast<UDataTable>(Loaded);
		if (!DT)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66CoreData] Exists but not a DataTable: %s"), *ObjectPath);
			return nullptr;
		}
		return DT;
	}

	UPackage* Package = CreatePackage(*PackagePath);
	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CoreData] Failed CreatePackage: %s"), *PackagePath);
		return nullptr;
	}

	UDataTable* NewDT = NewObject<UDataTable>(Package, UDataTable::StaticClass(), *AssetName, RF_Public | RF_Standalone);
	if (!NewDT)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CoreData] Failed NewObject<UDataTable>: %s"), *AssetName);
		return nullptr;
	}

	NewDT->RowStruct = RowStruct;

	FAssetRegistryModule::AssetCreated(NewDT);
	NewDT->MarkPackageDirty();
	UEditorAssetLibrary::SaveLoadedAsset(NewDT, true);

	UE_LOG(LogTemp, Display, TEXT("[T66CoreData] Created DT: %s | RowStruct=%s"), *ObjectPath, *GetNameSafe(RowStruct));
	return NewDT;
}

static UT66DataCatalogDA* EnsureCatalog(const FString& FolderPath, const FString& AssetName)
{
	UEditorAssetLibrary::MakeDirectory(FolderPath);

	const FString PackagePath = FolderPath + TEXT("/") + AssetName;
	const FString ObjectPath  = PackagePath + TEXT(".") + AssetName;

	if (UEditorAssetLibrary::DoesAssetExist(ObjectPath))
	{
		UObject* Loaded = UEditorAssetLibrary::LoadAsset(ObjectPath);
		UT66DataCatalogDA* Catalog = Cast<UT66DataCatalogDA>(Loaded);
		if (!Catalog)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66CoreData] Exists but not UT66DataCatalogDA: %s"), *ObjectPath);
			return nullptr;
		}
		return Catalog;
	}

	UPackage* Package = CreatePackage(*PackagePath);
	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CoreData] Failed CreatePackage for catalog: %s"), *PackagePath);
		return nullptr;
	}

	UT66DataCatalogDA* Catalog = NewObject<UT66DataCatalogDA>(Package, UT66DataCatalogDA::StaticClass(), *AssetName, RF_Public | RF_Standalone);
	if (!Catalog)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CoreData] Failed NewObject<UT66DataCatalogDA>: %s"), *AssetName);
		return nullptr;
	}

	FAssetRegistryModule::AssetCreated(Catalog);
	Catalog->MarkPackageDirty();
	UEditorAssetLibrary::SaveLoadedAsset(Catalog, true);

	UE_LOG(LogTemp, Display, TEXT("[T66CoreData] Created Catalog: %s"), *ObjectPath);
	return Catalog;
}

static bool ValidateDT(const TCHAR* Label, UDataTable* DT, UScriptStruct* Expected)
{
	if (!DT)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CoreData][VALIDATE] Missing %s"), Label);
		return false;
	}
	if (DT->GetRowStruct() != Expected)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CoreData][VALIDATE] Wrong RowStruct for %s. Expected=%s Actual=%s"),
			Label, *GetNameSafe(Expected), *GetNameSafe(DT->GetRowStruct()));
		return false;
	}
	UE_LOG(LogTemp, Display, TEXT("[T66CoreData][VALIDATE] OK %s"), Label);
	return true;
}

static bool ValidateCatalogPtr(const TCHAR* Label, const TSoftObjectPtr<UDataTable>& Ptr)
{
	if (Ptr.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CoreData][VALIDATE] Catalog missing pointer: %s"), Label);
		return false;
	}
	UDataTable* DT = Ptr.LoadSynchronous();
	if (!DT)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CoreData][VALIDATE] Catalog pointer could not load: %s"), Label);
		return false;
	}
	UE_LOG(LogTemp, Display, TEXT("[T66CoreData][VALIDATE] Catalog pointer OK: %s"), Label);
	return true;
}

bool FT66CoreDataTools::CreateOrRepairAndValidateCoreData(bool /*bForceOverwrite*/)
{
	const FString TablesRoot = TEXT("/Game/Tribulation66/Content/Data/Tables");
	const FString RegRoot    = TEXT("/Game/Tribulation66/Content/Data/Registries");

	// Core DTs
	UDataTable* DT_Items        = EnsureDataTable(TablesRoot + TEXT("/Items"),   TEXT("DT_Items"),        FT66ItemRow::StaticStruct());
	UDataTable* DT_Enemies      = EnsureDataTable(TablesRoot + TEXT("/Enemies"), TEXT("DT_Enemies"),      FT66EnemyRow::StaticStruct());
	UDataTable* DT_Bosses       = EnsureDataTable(TablesRoot + TEXT("/Bosses"),  TEXT("DT_Bosses"),       FT66BossRow::StaticStruct());
	UDataTable* DT_StageEffects = EnsureDataTable(TablesRoot + TEXT("/Stages"),  TEXT("DT_StageEffects"), FT66StageEffectRow::StaticStruct());
	UDataTable* DT_Companions   = EnsureDataTable(TablesRoot + TEXT("/Heroes"),  TEXT("DT_Companions"),   FT66CompanionRow::StaticStruct());

	// Supporting DTs
	UDataTable* DT_AttackPatterns   = EnsureDataTable(TablesRoot + TEXT("/Enemies"), TEXT("DT_AttackPatterns"),   FT66AttackPatternRow::StaticStruct());
	UDataTable* DT_MovementProfiles = EnsureDataTable(TablesRoot + TEXT("/Enemies"), TEXT("DT_MovementProfiles"), FT66MovementProfileRow::StaticStruct());
	UDataTable* DT_LuckSources      = EnsureDataTable(TablesRoot + TEXT("/Sec"),     TEXT("DT_LuckSources"),      FT66LuckSourceRow::StaticStruct());

	// Ultimates + White Items
	UDataTable* DT_Ultimates  = EnsureDataTable(TablesRoot + TEXT("/Combat"), TEXT("DT_Ultimates"),  FT66UltimateRow::StaticStruct());
	UDataTable* DT_WhiteItems = EnsureDataTable(TablesRoot + TEXT("/Items"),  TEXT("DT_WhiteItems"), FT66WhiteItemRow::StaticStruct());

	// Loot / drops
	UDataTable* DT_LootBagDrops  = EnsureDataTable(TablesRoot + TEXT("/Loot"), TEXT("DT_LootBagDrops"),  FT66LootBagDropRow::StaticStruct());
	UDataTable* DT_LootBagRarity = EnsureDataTable(TablesRoot + TEXT("/Loot"), TEXT("DT_LootBagRarity"), FT66LootBagRarityRow::StaticStruct());
	UDataTable* DT_LootPool      = EnsureDataTable(TablesRoot + TEXT("/Loot"), TEXT("DT_LootPool"),      FT66LootPoolRow::StaticStruct());

	// Gambling + debt
	UDataTable* DT_GambleOutcomes = EnsureDataTable(TablesRoot + TEXT("/Gambling"), TEXT("DT_GambleOutcomes"), FT66GambleOutcomeRow::StaticStruct());
	UDataTable* DT_LoanSharkRules = EnsureDataTable(TablesRoot + TEXT("/Economy"),  TEXT("DT_LoanSharkRules"), FT66LoanSharkRulesRow::StaticStruct());

	// Stages + heroes
	UDataTable* DT_Stages = EnsureDataTable(TablesRoot + TEXT("/Stages"), TEXT("DT_Stages"), FT66StageRow::StaticStruct());
	UDataTable* DT_Heroes = EnsureDataTable(TablesRoot + TEXT("/Heroes"), TEXT("DT_Heroes"), FT66HeroRow::StaticStruct());

	// Meta / progression
	UDataTable* DT_Idols        = EnsureDataTable(TablesRoot + TEXT("/Meta"), TEXT("DT_Idols"),        FT66IdolRow::StaticStruct());
	UDataTable* DT_Achievements = EnsureDataTable(TablesRoot + TEXT("/Meta"), TEXT("DT_Achievements"), FT66AchievementRow::StaticStruct());

	// Spawn director
	UDataTable* DT_SpawnWeighting = EnsureDataTable(TablesRoot + TEXT("/Director"), TEXT("DT_SpawnWeighting"), FT66SpawnWeightingRow::StaticStruct());

	UT66DataCatalogDA* Catalog = EnsureCatalog(RegRoot, TEXT("DA_DataCatalog_Core"));
	if (!Catalog)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CoreData] Failed to load/create DA_DataCatalog_Core"));
		return false;
	}

	// Wire catalog
	Catalog->Modify();
	Catalog->ItemsTable = DT_Items;
	Catalog->EnemiesTable = DT_Enemies;
	Catalog->BossesTable = DT_Bosses;
	Catalog->StageEffectsTable = DT_StageEffects;
	Catalog->CompanionsTable = DT_Companions;
	Catalog->AttackPatternsTable = DT_AttackPatterns;
	Catalog->MovementProfilesTable = DT_MovementProfiles;
	Catalog->LuckSourcesTable = DT_LuckSources;
	Catalog->UltimatesTable = DT_Ultimates;
	Catalog->WhiteItemsTable = DT_WhiteItems;
	Catalog->LootBagDropsTable = DT_LootBagDrops;
	Catalog->LootBagRarityTable = DT_LootBagRarity;
	Catalog->LootPoolTable = DT_LootPool;
	Catalog->GambleOutcomesTable = DT_GambleOutcomes;
	Catalog->LoanSharkRulesTable = DT_LoanSharkRules;
	Catalog->StagesTable = DT_Stages;
	Catalog->HeroesTable = DT_Heroes;
	Catalog->IdolsTable = DT_Idols;
	Catalog->AchievementsTable = DT_Achievements;
	Catalog->SpawnWeightingTable = DT_SpawnWeighting;

	Catalog->MarkPackageDirty();
	UEditorAssetLibrary::SaveLoadedAsset(Catalog, true);

	// Validate DT structs
	bool bOk = true;
	bOk &= ValidateDT(TEXT("DT_Items"), DT_Items, FT66ItemRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_Enemies"), DT_Enemies, FT66EnemyRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_Bosses"), DT_Bosses, FT66BossRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_StageEffects"), DT_StageEffects, FT66StageEffectRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_Companions"), DT_Companions, FT66CompanionRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_AttackPatterns"), DT_AttackPatterns, FT66AttackPatternRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_MovementProfiles"), DT_MovementProfiles, FT66MovementProfileRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_LuckSources"), DT_LuckSources, FT66LuckSourceRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_Ultimates"), DT_Ultimates, FT66UltimateRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_WhiteItems"), DT_WhiteItems, FT66WhiteItemRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_LootBagDrops"), DT_LootBagDrops, FT66LootBagDropRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_LootBagRarity"), DT_LootBagRarity, FT66LootBagRarityRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_LootPool"), DT_LootPool, FT66LootPoolRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_GambleOutcomes"), DT_GambleOutcomes, FT66GambleOutcomeRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_LoanSharkRules"), DT_LoanSharkRules, FT66LoanSharkRulesRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_Stages"), DT_Stages, FT66StageRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_Heroes"), DT_Heroes, FT66HeroRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_Idols"), DT_Idols, FT66IdolRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_Achievements"), DT_Achievements, FT66AchievementRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_SpawnWeighting"), DT_SpawnWeighting, FT66SpawnWeightingRow::StaticStruct());

	// Validate catalog pointers resolve
	bOk &= ValidateCatalogPtr(TEXT("ItemsTable"), Catalog->ItemsTable);
	bOk &= ValidateCatalogPtr(TEXT("EnemiesTable"), Catalog->EnemiesTable);
	bOk &= ValidateCatalogPtr(TEXT("BossesTable"), Catalog->BossesTable);
	bOk &= ValidateCatalogPtr(TEXT("StageEffectsTable"), Catalog->StageEffectsTable);
	bOk &= ValidateCatalogPtr(TEXT("CompanionsTable"), Catalog->CompanionsTable);
	bOk &= ValidateCatalogPtr(TEXT("AttackPatternsTable"), Catalog->AttackPatternsTable);
	bOk &= ValidateCatalogPtr(TEXT("MovementProfilesTable"), Catalog->MovementProfilesTable);
	bOk &= ValidateCatalogPtr(TEXT("LuckSourcesTable"), Catalog->LuckSourcesTable);
	bOk &= ValidateCatalogPtr(TEXT("UltimatesTable"), Catalog->UltimatesTable);
	bOk &= ValidateCatalogPtr(TEXT("WhiteItemsTable"), Catalog->WhiteItemsTable);
	bOk &= ValidateCatalogPtr(TEXT("LootBagDropsTable"), Catalog->LootBagDropsTable);
	bOk &= ValidateCatalogPtr(TEXT("LootBagRarityTable"), Catalog->LootBagRarityTable);
	bOk &= ValidateCatalogPtr(TEXT("LootPoolTable"), Catalog->LootPoolTable);
	bOk &= ValidateCatalogPtr(TEXT("GambleOutcomesTable"), Catalog->GambleOutcomesTable);
	bOk &= ValidateCatalogPtr(TEXT("LoanSharkRulesTable"), Catalog->LoanSharkRulesTable);
	bOk &= ValidateCatalogPtr(TEXT("StagesTable"), Catalog->StagesTable);
	bOk &= ValidateCatalogPtr(TEXT("HeroesTable"), Catalog->HeroesTable);
	bOk &= ValidateCatalogPtr(TEXT("IdolsTable"), Catalog->IdolsTable);
	bOk &= ValidateCatalogPtr(TEXT("AchievementsTable"), Catalog->AchievementsTable);
	bOk &= ValidateCatalogPtr(TEXT("SpawnWeightingTable"), Catalog->SpawnWeightingTable);

	UE_LOG(LogTemp, Display, TEXT("[T66CoreData][VALIDATE] Overall=%s"), bOk ? TEXT("PASS") : TEXT("FAIL"));
	return bOk;
}
