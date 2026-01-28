#include "T66WorldDataRepairCommandlet.h"

#include "AssetToolsModule.h"
#include "EditorAssetLibrary.h"
#include "Factories/DataTableFactory.h"
#include "Misc/ConfigCacheIni.h"
#include "Modules/ModuleManager.h"
#include "Engine/DataTable.h"

#include "T66/Data/Reg/T66DataCatalogDA.h"

// RowStructs
#include "T66/Data/Schema/World/T66InteractableProfileRow.h"
#include "T66/Data/Schema/World/T66SpinWheelRulesRow.h"
#include "T66/Data/Schema/World/T66CashTruckRulesRow.h"
#include "T66/Data/Schema/World/T66LifeTreeRulesRow.h"

#include "T66/Data/Schema/Enemy/Special/T66SpecialEnemyProfileRow.h"
#include "T66/Data/Schema/Enemy/Special/T66GoblinThiefRulesRow.h"
#include "T66/Data/Schema/Enemy/Special/T66LeprechaunRulesRow.h"

static bool EnsureDirectory(const FString& FolderPath)
{
	return UEditorAssetLibrary::DoesDirectoryExist(FolderPath) ? true : UEditorAssetLibrary::MakeDirectory(FolderPath);
}

static UDataTable* EnsureDataTableAsset(const FString& FolderPath, const FString& AssetName, UScriptStruct* RowStruct, int32& InOutCreatedCount)
{
	EnsureDirectory(FolderPath);

	const FString PackagePath = FolderPath / AssetName;
	const FString ObjectPath  = PackagePath + TEXT(".") + AssetName;

	if (UEditorAssetLibrary::DoesAssetExist(ObjectPath))
	{
		return Cast<UDataTable>(UEditorAssetLibrary::LoadAsset(ObjectPath));
	}

	UDataTableFactory* Factory = NewObject<UDataTableFactory>();
	Factory->Struct = RowStruct;

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	UObject* NewAsset = AssetToolsModule.Get().CreateAsset(AssetName, FolderPath, UDataTable::StaticClass(), Factory);

	UDataTable* DT = Cast<UDataTable>(NewAsset);
	if (DT)
	{
		DT->MarkPackageDirty();
		UEditorAssetLibrary::SaveLoadedAsset(DT, true);
		InOutCreatedCount++;
	}

	return DT;
}

static bool ValidateDT(const TCHAR* Label, UDataTable* DT, UScriptStruct* ExpectedStruct)
{
	if (!DT)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66WorldData][VALIDATE] %s = FAIL (null)"), Label);
		return false;
	}

	if (DT->GetRowStruct() != ExpectedStruct)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66WorldData][VALIDATE] %s = FAIL (RowStruct mismatch). Got=%s Expected=%s"),
			Label,
			DT->GetRowStruct() ? *DT->GetRowStruct()->GetName() : TEXT("null"),
			ExpectedStruct ? *ExpectedStruct->GetName() : TEXT("null"));
		return false;
	}

	UE_LOG(LogTemp, Display, TEXT("[T66WorldData][VALIDATE] %s = PASS"), Label);
	return true;
}

UT66WorldDataRepairCommandlet::UT66WorldDataRepairCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66WorldDataRepairCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Display, TEXT("[T66WorldData] Repair start"));

	// Load DataCatalog from config
	FString CatalogAssetPath;
	if (!GConfig->GetString(TEXT("/Script/T66.T66DataCatalogSubsystem"), TEXT("DataCatalogAssetPath"), CatalogAssetPath, GGameIni))
	{
		UE_LOG(LogTemp, Error, TEXT("[T66WorldData] Missing config: [/Script/T66.T66DataCatalogSubsystem] DataCatalogAssetPath"));
		return 1;
	}

	UObject* CatalogObj = StaticLoadObject(UObject::StaticClass(), nullptr, *CatalogAssetPath);
	UT66DataCatalogDA* Catalog = Cast<UT66DataCatalogDA>(CatalogObj);
	if (!Catalog)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66WorldData] Failed to load Catalog as UT66DataCatalogDA: %s"), *CatalogAssetPath);
		return 1;
	}

	const FString WorldRoot = TEXT("/Game/Tribulation66/Content/Data/Tables/World");
	const FString InterRoot = WorldRoot / TEXT("Interactables");
	const FString SpecRoot  = TEXT("/Game/Tribulation66/Content/Data/Tables/Enemies/Special");

	int32 Created = 0;

	// World Interactables
	UDataTable* DT_Interactables   = EnsureDataTableAsset(InterRoot, TEXT("DT_Interactables"), FT66InteractableProfileRow::StaticStruct(), Created);
	UDataTable* DT_SpinWheelRules  = EnsureDataTableAsset(InterRoot / TEXT("SpinWheel"), TEXT("DT_SpinWheelRules"), FT66SpinWheelRulesRow::StaticStruct(), Created);
	UDataTable* DT_CashTruckRules  = EnsureDataTableAsset(InterRoot / TEXT("CashTruck"), TEXT("DT_CashTruckRules"), FT66CashTruckRulesRow::StaticStruct(), Created);
	UDataTable* DT_LifeTreeRules   = EnsureDataTableAsset(InterRoot / TEXT("LifeTree"),  TEXT("DT_LifeTreeRules"),  FT66LifeTreeRulesRow::StaticStruct(), Created);

	// Special Enemies
	UDataTable* DT_SpecialEnemies  = EnsureDataTableAsset(SpecRoot, TEXT("DT_SpecialEnemies"), FT66SpecialEnemyProfileRow::StaticStruct(), Created);
	UDataTable* DT_GoblinRules     = EnsureDataTableAsset(SpecRoot / TEXT("GoblinThief"), TEXT("DT_GoblinThiefRules"), FT66GoblinThiefRulesRow::StaticStruct(), Created);
	UDataTable* DT_LeprechaunRules = EnsureDataTableAsset(SpecRoot / TEXT("Leprechaun"),  TEXT("DT_LeprechaunRules"),  FT66LeprechaunRulesRow::StaticStruct(), Created);

	// Wire into catalog
	Catalog->Modify();

	Catalog->InteractablesTable        = DT_Interactables;
	Catalog->SpinWheelRulesTable       = DT_SpinWheelRules;
	Catalog->CashTruckRulesTable       = DT_CashTruckRules;
	Catalog->LifeTreeRulesTable        = DT_LifeTreeRules;

	Catalog->SpecialEnemyProfilesTable = DT_SpecialEnemies;
	Catalog->GoblinThiefRulesTable     = DT_GoblinRules;
	Catalog->LeprechaunRulesTable      = DT_LeprechaunRules;

	Catalog->MarkPackageDirty();
	UEditorAssetLibrary::SaveLoadedAsset(Catalog, true);

	// Validate
	bool bOk = true;
	bOk &= ValidateDT(TEXT("DT_Interactables"),     DT_Interactables,   FT66InteractableProfileRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_SpinWheelRules"),    DT_SpinWheelRules,  FT66SpinWheelRulesRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_CashTruckRules"),    DT_CashTruckRules,  FT66CashTruckRulesRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_LifeTreeRules"),     DT_LifeTreeRules,   FT66LifeTreeRulesRow::StaticStruct());

	bOk &= ValidateDT(TEXT("DT_SpecialEnemies"),    DT_SpecialEnemies,  FT66SpecialEnemyProfileRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_GoblinThiefRules"),  DT_GoblinRules,     FT66GoblinThiefRulesRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_LeprechaunRules"),   DT_LeprechaunRules, FT66LeprechaunRulesRow::StaticStruct());

	UE_LOG(LogTemp, Display, TEXT("[T66WorldData] Created=%d"), Created);
	UE_LOG(LogTemp, Display, TEXT("[T66WorldData][VALIDATE] Overall=%s"), bOk ? TEXT("PASS") : TEXT("FAIL"));

	return bOk ? 0 : 1;
}
