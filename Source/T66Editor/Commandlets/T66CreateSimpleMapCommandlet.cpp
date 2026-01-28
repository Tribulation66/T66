#include "T66CreateSimpleMapCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EditorAssetLibrary.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "FileHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/SavePackage.h"

static const TCHAR* MAP_PATH = TEXT("/Game/Tribulation66/Content/Maps/Lvl_Simple");
static const TCHAR* CUBE_MESH = TEXT("/Engine/BasicShapes/Cube.Cube");
static const TCHAR* FLOOR_MATERIAL = TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial");

UT66CreateSimpleMapCommandlet::UT66CreateSimpleMapCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66CreateSimpleMapCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Display, TEXT("[T66CreateSimpleMap] Starting level creation..."));

	// Ensure content directory exists
	const FString ContentDir = FPaths::ProjectContentDir() / TEXT("Tribulation66/Content/Maps");
	IFileManager::Get().MakeDirectory(*ContentDir, true);

	// Package setup
	const FString PackageName = FString(MAP_PATH);
	const FString MapName = FPackageName::GetShortName(PackageName);
	const FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetMapPackageExtension());

	// Create package
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CreateSimpleMap] Failed to create package"));
		return 1;
	}
	Package->FullyLoad();
	Package->SetPackageFlags(PKG_NewlyCreated);

	// Create world
	UWorld* World = UWorld::CreateWorld(EWorldType::Inactive, false, FName(*MapName), Package);
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CreateSimpleMap] Failed to create world"));
		return 1;
	}

	// Don't initialize - just set up minimal structure
	World->SetFlags(RF_Public | RF_Standalone);
	World->ClearFlags(RF_Transient);
	
	ULevel* Level = World->PersistentLevel;
	if (!Level)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CreateSimpleMap] No persistent level"));
		return 1;
	}

	// Load assets
	UStaticMesh* CubeMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, CUBE_MESH));
	
	// We need to spawn actors properly using SpawnActor in an initialized world context
	// Create a temp world context
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Editor);
	WorldContext.SetCurrentWorld(World);
	
	// Now the world has a context, but we still can't spawn actors easily without full init
	// Let's try a different approach - just save an empty level first
	
	// Clean up world context before save
	GEngine->DestroyWorldContext(World);

	// Mark for save
	Package->MarkPackageDirty();
	
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError | SAVE_KeepDirty;
	
	bool bSaved = UPackage::Save(Package, World, *FilePath, SaveArgs).IsSuccessful();

	if (bSaved)
	{
		UE_LOG(LogTemp, Display, TEXT("[T66CreateSimpleMap] Created empty level at: %s"), *FilePath);
		UE_LOG(LogTemp, Display, TEXT("[T66CreateSimpleMap] "));
		UE_LOG(LogTemp, Display, TEXT("[T66CreateSimpleMap] NEXT STEP: Open the editor and manually add:"));
		UE_LOG(LogTemp, Display, TEXT("[T66CreateSimpleMap]   1. A floor (Cube scaled 100x100x1)"));
		UE_LOG(LogTemp, Display, TEXT("[T66CreateSimpleMap]   2. A Directional Light"));
		UE_LOG(LogTemp, Display, TEXT("[T66CreateSimpleMap]   3. A Sky Light"));
		UE_LOG(LogTemp, Display, TEXT("[T66CreateSimpleMap]   4. A Sky Atmosphere"));
		return 0;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[T66CreateSimpleMap] Failed to save level"));
		return 1;
	}
}
