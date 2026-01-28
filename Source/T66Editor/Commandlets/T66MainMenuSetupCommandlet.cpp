#include "T66MainMenuSetupCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorAssetLibrary.h"
#include "Factories/WorldFactory.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

static const TCHAR* T66_MAINMENU_MAP_DIR = TEXT("/Game/Tribulation66/Content/Maps");
static const TCHAR* T66_MAINMENU_MAP_NAME = TEXT("Lvl_MainMenu");

namespace
{
	static bool CreateMainMenuMapIfMissing()
	{
		const FString FolderPath = T66_MAINMENU_MAP_DIR;
		const FString AssetName = T66_MAINMENU_MAP_NAME;
		const FString PackagePath = FolderPath + TEXT("/") + AssetName;
		const FString ObjectPath = PackagePath + TEXT(".") + AssetName;

		if (UEditorAssetLibrary::DoesAssetExist(ObjectPath))
		{
			UE_LOG(LogTemp, Display, TEXT("[T66MainMenuSetup] Map already exists: %s"), *ObjectPath);
			return true;
		}

		UEditorAssetLibrary::MakeDirectory(FolderPath);

		UPackage* Package = CreatePackage(*PackagePath);
		if (!Package)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66MainMenuSetup] Failed CreatePackage: %s"), *PackagePath);
			return false;
		}

		UWorldFactory* Factory = NewObject<UWorldFactory>();
		Factory->WorldType = EWorldType::Game;

		UObject* NewAsset = Factory->FactoryCreateNew(
			UWorld::StaticClass(),
			Package,
			*AssetName,
			RF_Public | RF_Standalone,
			nullptr,
			GWarn);

		if (!NewAsset)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66MainMenuSetup] Failed to create map asset: %s"), *ObjectPath);
			return false;
		}

		FAssetRegistryModule::AssetCreated(NewAsset);
		Package->MarkPackageDirty();

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_None;
		const bool bSaved = UPackage::SavePackage(Package, NewAsset, *FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetMapPackageExtension()), SaveArgs);

		UE_LOG(LogTemp, Display, TEXT("[T66MainMenuSetup] Created map: %s | Saved=%s"), *ObjectPath, bSaved ? TEXT("true") : TEXT("false"));
		return bSaved;
	}
}

UT66MainMenuSetupCommandlet::UT66MainMenuSetupCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66MainMenuSetupCommandlet::Main(const FString& Params)
{
	const bool bOk = CreateMainMenuMapIfMissing();
	return bOk ? 0 : 1;
}
