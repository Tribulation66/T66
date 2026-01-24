#include "T66UIShellSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Engine/World.h"

// ✅ This is where FCoreUObjectDelegates exists in UE5.x
#include "UObject/UObjectGlobals.h"

static const TCHAR* T66_UIROOT_PATH = TEXT("/Game/Tribulation66/Content/UI/Root/WBP_UIRoot.WBP_UIRoot_C");

void UT66UIShellSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// ✅ UE5-safe hook after map load (works in PIE and Game)
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this,
		&UT66UIShellSubsystem::HandlePostLoadMapWithWorld
	);

	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Initialized"));
}

void UT66UIShellSubsystem::Deinitialize()
{
	if (PostLoadMapHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
		PostLoadMapHandle.Reset();
	}

	SpawnedUIRoot = nullptr;

	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Deinitialized"));

	Super::Deinitialize();
}

void UT66UIShellSubsystem::HandlePostLoadMapWithWorld(UWorld* LoadedWorld)
{
	if (!LoadedWorld)
	{
		return;
	}

	// Only in real play worlds (PIE/Game) — skip editor preview worlds
	const EWorldType::Type WT = LoadedWorld->WorldType;
	const bool bPlayableWorld = (WT == EWorldType::PIE) || (WT == EWorldType::Game);

	if (!bPlayableWorld)
	{
		return;
	}

	SpawnUIRoot(LoadedWorld);
}

void UT66UIShellSubsystem::SpawnUIRoot(UWorld* World)
{
	if (!World)
	{
		return;
	}

	// Only spawn once
	if (SpawnedUIRoot)
	{
		return;
	}

	UClass* RootWidgetClass = StaticLoadClass(UUserWidget::StaticClass(), nullptr, T66_UIROOT_PATH);
	if (!RootWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66UIShellSubsystem] Failed to load WBP_UIRoot class at: %s"), T66_UIROOT_PATH);
		return;
	}

	UUserWidget* RootWidget = CreateWidget<UUserWidget>(World, RootWidgetClass);
	if (!RootWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66UIShellSubsystem] Failed to create WBP_UIRoot instance"));
		return;
	}

	RootWidget->AddToViewport(/*ZOrder=*/0);
	SpawnedUIRoot = RootWidget;

	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Spawned WBP_UIRoot into viewport"));
}
