#include "T66NPCDataRepairCommandlet.h"

#include "AssetToolsModule.h"
#include "EditorAssetLibrary.h"
#include "Factories/DataTableFactory.h"
#include "Misc/ConfigCacheIni.h"
#include "Modules/ModuleManager.h"

#include "Engine/DataTable.h"
#include "UObject/SoftObjectPath.h"

#include "T66/Data/Reg/T66DataCatalogDA.h"

// RowStructs
#include "T66/Data/Schema/NPC/T66NPCProfileRow.h"
#include "T66/Data/Schema/NPC/T66VendorRulesRow.h"
#include "T66/Data/Schema/NPC/T66GamblerRulesRow.h"
#include "T66/Data/Schema/NPC/T66SaintRulesRow.h"
#include "T66/Data/Schema/NPC/T66TricksterRulesRow.h"
#include "T66/Data/Schema/NPC/T66OuroborosRulesRow.h"
#include "T66/Data/Schema/NPC/T66StealMatrixRow.h"
#include "T66/Data/Schema/NPC/T66CheatMatrixRow.h"

static bool EnsureDirectory(const FString& FolderPath)
{
	if (UEditorAssetLibrary::DoesDirectoryExist(FolderPath))
	{
		return true;
	}
	return UEditorAssetLibrary::MakeDirectory(FolderPath);
}

static UDataTable* EnsureDataTableAsset(const FString& FolderPath, const FString& AssetName, UScriptStruct* RowStruct, int32& InOutCreatedCount)
{
	EnsureDirectory(FolderPath);

	const FString PackagePath = FolderPath / AssetName;
	const FString ObjectPath = PackagePath + TEXT(".") + AssetName;

	if (UEditorAssetLibrary::DoesAssetExist(ObjectPath))
	{
		return Cast<UDataTable>(UEditorAssetLibrary::LoadAsset(ObjectPath));
	}

	UDataTableFactory* Factory = NewObject<UDataTableFactory>();
	Factory->Struct = RowStruct;

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	UObject* NewAsset = AssetToolsModule.Get().CreateAsset(AssetName, FolderPath, UDataTable::StaticClass(), Factory);

	if (UDataTable* DT = Cast<UDataTable>(NewAsset))
	{
		DT->MarkPackageDirty();
		UEditorAssetLibrary::SaveLoadedAsset(DT, true);
		InOutCreatedCount++;
		return DT;
	}

	return nullptr;
}

static bool ValidateDT(const TCHAR* Label, UDataTable* DT, UScriptStruct* ExpectedStruct)
{
	if (!DT)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66NPCData][VALIDATE] %s = FAIL (null)"), Label);
		return false;
	}

	if (DT->GetRowStruct() != ExpectedStruct)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66NPCData][VALIDATE] %s = FAIL (RowStruct mismatch). Got=%s Expected=%s"),
			Label,
			DT->GetRowStruct() ? *DT->GetRowStruct()->GetName() : TEXT("null"),
			ExpectedStruct ? *ExpectedStruct->GetName() : TEXT("null"));
		return false;
	}

	UE_LOG(LogTemp, Display, TEXT("[T66NPCData][VALIDATE] %s = PASS"), Label);
	return true;
}

UT66NPCDataRepairCommandlet::UT66NPCDataRepairCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66NPCDataRepairCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Display, TEXT("[T66NPCData] Repair start"));

	// 1) Read DataCatalogAssetPath from config (same section you already use)
	FString CatalogAssetPath;
	if (!GConfig->GetString(TEXT("/Script/T66.T66DataCatalogSubsystem"), TEXT("DataCatalogAssetPath"), CatalogAssetPath, GGameIni))
	{
		UE_LOG(LogTemp, Error, TEXT("[T66NPCData] Missing config: [/Script/T66.T66DataCatalogSubsystem] DataCatalogAssetPath"));
		return 1;
	}

	UObject* CatalogObj = StaticLoadObject(UObject::StaticClass(), nullptr, *CatalogAssetPath);
	UT66DataCatalogDA* Catalog = Cast<UT66DataCatalogDA>(CatalogObj);
	if (!Catalog)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66NPCData] Failed to load Catalog as UT66DataCatalogDA: %s"), *CatalogAssetPath);
		return 1;
	}

	// 2) Create/Repair the DT assets (canonical folders)
	const FString RootTables = TEXT("/Game/Tribulation66/Content/Data/Tables/NPC");

	int32 Created = 0;

	UDataTable* DT_NPCProfiles   = EnsureDataTableAsset(RootTables, TEXT("DT_NPCProfiles"),   FT66NPCProfileRow::StaticStruct(), Created);

	UDataTable* DT_VendorRules   = EnsureDataTableAsset(RootTables / TEXT("Vendor"),   TEXT("DT_VendorRules"),   FT66VendorRulesRow::StaticStruct(), Created);
	UDataTable* DT_StealMatrix   = EnsureDataTableAsset(RootTables / TEXT("Vendor"),   TEXT("DT_StealMatrix"),   FT66StealMatrixRow::StaticStruct(), Created);

	UDataTable* DT_GamblerRules  = EnsureDataTableAsset(RootTables / TEXT("Gambler"),  TEXT("DT_GamblerRules"),  FT66GamblerRulesRow::StaticStruct(), Created);
	UDataTable* DT_CheatMatrix   = EnsureDataTableAsset(RootTables / TEXT("Gambler"),  TEXT("DT_CheatMatrix"),   FT66CheatMatrixRow::StaticStruct(), Created);

	UDataTable* DT_SaintRules    = EnsureDataTableAsset(RootTables / TEXT("Saint"),    TEXT("DT_SaintRules"),    FT66SaintRulesRow::StaticStruct(), Created);
	UDataTable* DT_TricksterRules= EnsureDataTableAsset(RootTables / TEXT("Trickster"),TEXT("DT_TricksterRules"),FT66TricksterRulesRow::StaticStruct(), Created);
	UDataTable* DT_OuroborosRules= EnsureDataTableAsset(RootTables / TEXT("Ouroboros"),TEXT("DT_OuroborosRules"),FT66OuroborosRulesRow::StaticStruct(), Created);

	// 3) Assign into DataCatalog (soft pointers)
	Catalog->Modify();

	Catalog->NPCProfilesTable    = DT_NPCProfiles;

	Catalog->VendorRulesTable    = DT_VendorRules;
	Catalog->StealMatrixTable    = DT_StealMatrix;

	Catalog->GamblerRulesTable   = DT_GamblerRules;
	Catalog->CheatMatrixTable    = DT_CheatMatrix;

	Catalog->SaintRulesTable     = DT_SaintRules;
	Catalog->TricksterRulesTable = DT_TricksterRules;
	Catalog->OuroborosRulesTable = DT_OuroborosRules;

	Catalog->MarkPackageDirty();
	UEditorAssetLibrary::SaveLoadedAsset(Catalog, true);

	// 4) Validate
	bool bOk = true;
	bOk &= ValidateDT(TEXT("DT_NPCProfiles"),    DT_NPCProfiles,    FT66NPCProfileRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_VendorRules"),    DT_VendorRules,    FT66VendorRulesRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_StealMatrix"),    DT_StealMatrix,    FT66StealMatrixRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_GamblerRules"),   DT_GamblerRules,   FT66GamblerRulesRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_CheatMatrix"),    DT_CheatMatrix,    FT66CheatMatrixRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_SaintRules"),     DT_SaintRules,     FT66SaintRulesRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_TricksterRules"), DT_TricksterRules, FT66TricksterRulesRow::StaticStruct());
	bOk &= ValidateDT(TEXT("DT_OuroborosRules"), DT_OuroborosRules, FT66OuroborosRulesRow::StaticStruct());

	UE_LOG(LogTemp, Display, TEXT("[T66NPCData] Created=%d"), Created);
	UE_LOG(LogTemp, Display, TEXT("[T66NPCData][VALIDATE] Overall=%s"), bOk ? TEXT("PASS") : TEXT("FAIL"));

	return bOk ? 0 : 1;
}
