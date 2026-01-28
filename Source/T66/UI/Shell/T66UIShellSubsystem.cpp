#include "T66UIShellSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/PackageName.h"
#include "Engine/GameInstance.h"
#include "T66SelectionSubsystem.h"
#include "T66PreviewSubsystem.h"

// Screen paths
static const TCHAR* SCREEN_MAINMENU = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_MainMenu.WBP_Screen_MainMenu_C");
static const TCHAR* SCREEN_PARTYSIZE = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_PartySizePicker.WBP_Screen_PartySizePicker_C");
static const TCHAR* SCREEN_HEROSELECT_SOLO = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_HeroSelect_Solo.WBP_Screen_HeroSelect_Solo_C");
static const TCHAR* SCREEN_HEROSELECT_COOP = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_HeroSelect_Coop.WBP_Screen_HeroSelect_Coop_C");
static const TCHAR* SCREEN_SAVESLOTS = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_SaveSlots.WBP_Screen_SaveSlots_C");
static const TCHAR* SCREEN_ACHIEVEMENTS = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_Achievements.WBP_Screen_Achievements_C");
static const TCHAR* SCREEN_ACCOUNTSTATUS = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_AccountStatus.WBP_Screen_AccountStatus_C");
static const TCHAR* SCREEN_COMPANIONSELECT = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_CompanionSelect.WBP_Screen_CompanionSelect_C");

// Use our simple level with proper lighting
static const FName GAMEPLAY_MAP_NAME = FName(TEXT("Lvl_Simple"));

void UT66UIShellSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnWorldInitHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(
		this,
		&UT66UIShellSubsystem::HandleWorldInit
	);

	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Initialized"));
}

void UT66UIShellSubsystem::Deinitialize()
{
	if (OnWorldInitHandle.IsValid())
	{
		FWorldDelegates::OnPostWorldInitialization.Remove(OnWorldInitHandle);
		OnWorldInitHandle.Reset();
	}

	CurrentScreen = nullptr;
	ScreenStack.Empty();

	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Deinitialized"));

	Super::Deinitialize();
}

void UT66UIShellSubsystem::HandleWorldInit(UWorld* World, const UWorld::InitializationValues IVS)
{
	if (!World)
	{
		return;
	}

	const EWorldType::Type WT = World->WorldType;
	const bool bPlayableWorld = (WT == EWorldType::PIE) || (WT == EWorldType::Game);

	if (!bPlayableWorld)
	{
		return;
	}

	// Store world reference
	CurrentWorld = World;

	// If we're entering gameplay, don't spawn the main menu
	if (bEnteringGameplay)
	{
		UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Entered gameplay level - skipping main menu spawn"));
		bEnteringGameplay = false;
		CurrentScreen = nullptr;
		ScreenStack.Empty();
		return;
	}

	// Reset state for new PIE session
	CurrentScreen = nullptr;
	ScreenStack.Empty();

	// Delay spawn to next frame to ensure viewport is ready
	World->GetTimerManager().SetTimerForNextTick(this, &UT66UIShellSubsystem::SpawnMainMenuDeferred);
}

void UT66UIShellSubsystem::SpawnMainMenuDeferred()
{
	UWorld* World = CurrentWorld.Get();
	if (!World)
	{
		return;
	}

	SpawnMainMenu(World);
}

void UT66UIShellSubsystem::SpawnMainMenu(UWorld* World)
{
	if (!World)
	{
		return;
	}

	UUserWidget* MainMenu = SpawnScreen(SCREEN_MAINMENU, 100);
	if (MainMenu)
	{
		// Delay button binding slightly to ensure widget is fully constructed
		World->GetTimerManager().SetTimer(
			ButtonBindTimer,
			this,
			&UT66UIShellSubsystem::BindMainMenuButtons,
			0.1f,
			false
		);
	}
}

UUserWidget* UT66UIShellSubsystem::SpawnScreen(const FString& ScreenPath, int32 ZOrder)
{
	UWorld* World = CurrentWorld.Get();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66UIShellSubsystem] SpawnScreen: No world"));
		return nullptr;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66UIShellSubsystem] SpawnScreen: No PlayerController"));
		return nullptr;
	}

	UClass* ScreenClass = StaticLoadClass(UUserWidget::StaticClass(), nullptr, *ScreenPath);
	if (!ScreenClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66UIShellSubsystem] Failed to load screen class: %s"), *ScreenPath);
		return nullptr;
	}

	UUserWidget* ScreenWidget = CreateWidget<UUserWidget>(PC, ScreenClass);
	if (!ScreenWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66UIShellSubsystem] Failed to create screen widget: %s"), *ScreenPath);
		return nullptr;
	}

	ScreenWidget->AddToViewport(ZOrder);
	
	// Push to stack
	if (CurrentScreen)
	{
		ScreenStack.Push(CurrentScreen);
	}
	CurrentScreen = ScreenWidget;

	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Spawned screen: %s"), *ScreenPath);
	return ScreenWidget;
}

void UT66UIShellSubsystem::CloseCurrentScreen()
{
	if (CurrentScreen)
	{
		CurrentScreen->RemoveFromParent();
		CurrentScreen = nullptr;
	}

	// Pop previous screen from stack
	if (ScreenStack.Num() > 0)
	{
		CurrentScreen = ScreenStack.Pop();
	}
}

void UT66UIShellSubsystem::NavigateToScreen(const FString& ScreenPath)
{
	SpawnScreen(ScreenPath, 110);
}

void UT66UIShellSubsystem::NavigateBack()
{
	CloseCurrentScreen();
}

void UT66UIShellSubsystem::StartGameplay()
{
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		// Set flag to prevent main menu from spawning on the gameplay level
		bEnteringGameplay = true;
		
		// Use our C++ simple game mode - no Blueprint dependencies
		const FString Options = TEXT("?game=/Script/T66.T66SimpleGameMode");
		UGameplayStatics::OpenLevel(World, GAMEPLAY_MAP_NAME, true, Options);
		UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Opening simple test level with C++ character"));
	}
}

// Recursive helper to find buttons
static void FindButtonsRecursive(UWidget* Widget, const FString& NameContains, TArray<UButton*>& OutButtons)
{
	if (!Widget)
	{
		return;
	}

	// Check if this is a button with matching name
	if (Widget->GetName().Contains(NameContains))
	{
		if (UButton* Button = Cast<UButton>(Widget))
		{
			OutButtons.Add(Button);
		}
	}

	// Recurse into panel children
	if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
	{
		for (int32 i = 0; i < Panel->GetChildrenCount(); ++i)
		{
			FindButtonsRecursive(Panel->GetChildAt(i), NameContains, OutButtons);
		}
	}
}

UButton* UT66UIShellSubsystem::FindButtonInMenu(const FString& ButtonNameContains)
{
	if (!CurrentScreen || !CurrentScreen->WidgetTree)
	{
		return nullptr;
	}

	TArray<UButton*> Buttons;
	FindButtonsRecursive(CurrentScreen->WidgetTree->RootWidget, ButtonNameContains, Buttons);

	return Buttons.Num() > 0 ? Buttons[0] : nullptr;
}

void UT66UIShellSubsystem::BindMainMenuButtons()
{
	if (!CurrentScreen)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66UIShellSubsystem] BindMainMenuButtons: No current screen"));
		return;
	}

	int32 BoundCount = 0;

	// Bind each main menu button
	if (UButton* Btn = FindButtonInMenu(TEXT("NewGame_Btn")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnNewGameClicked);
		BoundCount++;
	}

	if (UButton* Btn = FindButtonInMenu(TEXT("LoadGame_Btn")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnLoadGameClicked);
		BoundCount++;
	}

	if (UButton* Btn = FindButtonInMenu(TEXT("Settings_Btn")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnSettingsClicked);
		BoundCount++;
	}

	if (UButton* Btn = FindButtonInMenu(TEXT("Achievements_Btn")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnAchievementsClicked);
		BoundCount++;
	}

	if (UButton* Btn = FindButtonInMenu(TEXT("AccountStatus_Btn")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnAccountStatusClicked);
		BoundCount++;
	}

	if (UButton* Btn = FindButtonInMenu(TEXT("LanguageIcon_Btn")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnLanguageClicked);
		BoundCount++;
	}

	if (UButton* Btn = FindButtonInMenu(TEXT("QuitIcon_Btn")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnQuitClicked);
		BoundCount++;
	}

	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Bound %d main menu buttons"), BoundCount);
}

void UT66UIShellSubsystem::BindBackButton()
{
	if (UButton* BackBtn = FindButtonInMenu(TEXT("Back_Btn")))
	{
		BackBtn->OnClicked.RemoveAll(this);
		BackBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnBackClicked);
		UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Bound Back button"));
	}
}

void UT66UIShellSubsystem::OnBackClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Back clicked"));
	
	// Close current screen
	if (CurrentScreen)
	{
		CurrentScreen->RemoveFromParent();
		CurrentScreen = nullptr;
	}
	
	// Pop previous screen or return to main menu
	if (ScreenStack.Num() > 0)
	{
		CurrentScreen = ScreenStack.Pop();
		UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Returned to previous screen"));
	}
	else
	{
		// Return to main menu
		SpawnMainMenu(CurrentWorld.Get());
	}
}

void UT66UIShellSubsystem::OnNewGameClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] New Game clicked -> Party Size Picker"));
	
	// Close main menu and open party size picker
	CloseCurrentScreen();
	UUserWidget* PartySizeScreen = SpawnScreen(SCREEN_PARTYSIZE, 100);
	
	if (PartySizeScreen)
	{
		// Defer binding for the party size buttons
		UWorld* World = CurrentWorld.Get();
		if (World)
		{
			World->GetTimerManager().SetTimerForNextTick([this]()
			{
				// Bind Solo button
				if (UButton* SoloBtn = FindButtonInMenu(TEXT("Solo_Btn")))
				{
					SoloBtn->OnClicked.RemoveAll(this);
					SoloBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnSoloClicked);
				}
				// Bind Duo button
				if (UButton* DuoBtn = FindButtonInMenu(TEXT("Duo_Btn")))
				{
					DuoBtn->OnClicked.RemoveAll(this);
					DuoBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnDuoClicked);
				}
				// Bind Trio button
				if (UButton* TrioBtn = FindButtonInMenu(TEXT("Trio_Btn")))
				{
					TrioBtn->OnClicked.RemoveAll(this);
					TrioBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnTrioClicked);
				}
				// Bind Back button
				BindBackButton();
			});
		}
	}
}

void UT66UIShellSubsystem::OnLoadGameClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Load Game clicked -> Save Slots"));
	CloseCurrentScreen();
	SpawnScreen(SCREEN_SAVESLOTS, 100);
	
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		World->GetTimerManager().SetTimerForNextTick([this]() { BindBackButton(); });
	}
}

void UT66UIShellSubsystem::OnSettingsClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Settings clicked"));
	// Settings is typically a modal overlay, keep main menu visible
	// For now, just log - can implement settings modal later
}

void UT66UIShellSubsystem::OnAchievementsClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Achievements clicked"));
	CloseCurrentScreen();
	SpawnScreen(SCREEN_ACHIEVEMENTS, 100);
	
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		World->GetTimerManager().SetTimerForNextTick([this]() { BindBackButton(); });
	}
}

void UT66UIShellSubsystem::OnAccountStatusClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Account Status clicked"));
	CloseCurrentScreen();
	SpawnScreen(SCREEN_ACCOUNTSTATUS, 100);
	
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		World->GetTimerManager().SetTimerForNextTick([this]() { BindBackButton(); });
	}
}

void UT66UIShellSubsystem::OnLanguageClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Language clicked"));
	// Language selector modal - not implemented yet
}

void UT66UIShellSubsystem::OnQuitClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Quit clicked"));
	
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		// In PIE, just request exit
		UKismetSystemLibrary::QuitGame(World, World->GetFirstPlayerController(), EQuitPreference::Quit, false);
	}
}

void UT66UIShellSubsystem::OnSoloClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Solo clicked -> Hero Select Solo"));
	CloseCurrentScreen();
	SpawnScreen(SCREEN_HEROSELECT_SOLO, 100);
	
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		World->GetTimerManager().SetTimerForNextTick([this]()
		{
			BindHeroSelectionButtons();
			BindBackButton();
		});
	}
}

void UT66UIShellSubsystem::OnDuoClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Duo clicked -> Hero Select Coop (Duo)"));
	CloseCurrentScreen();
	SpawnScreen(SCREEN_HEROSELECT_COOP, 100);
	
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		World->GetTimerManager().SetTimerForNextTick([this]()
		{
			BindHeroSelectionButtons();
			BindBackButton();
		});
	}
}

void UT66UIShellSubsystem::OnTrioClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Trio clicked -> Hero Select Coop (Trio)"));
	CloseCurrentScreen();
	SpawnScreen(SCREEN_HEROSELECT_COOP, 100);
	
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		World->GetTimerManager().SetTimerForNextTick([this]()
		{
			BindHeroSelectionButtons();
			BindBackButton();
		});
	}
}

void UT66UIShellSubsystem::BindHeroSelectionButtons()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Binding hero selection buttons..."));
	
	// Bind individual hero buttons
	if (UButton* Btn = FindButtonInMenu(TEXT("Btn_Hero_0")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnHero0Clicked);
	}
	if (UButton* Btn = FindButtonInMenu(TEXT("Btn_Hero_1")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnHero1Clicked);
	}
	if (UButton* Btn = FindButtonInMenu(TEXT("Btn_Hero_2")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnHero2Clicked);
	}
	if (UButton* Btn = FindButtonInMenu(TEXT("Btn_Hero_3")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnHero3Clicked);
	}
	if (UButton* Btn = FindButtonInMenu(TEXT("Btn_Hero_4")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnHero4Clicked);
	}
	if (UButton* Btn = FindButtonInMenu(TEXT("Btn_Hero_5")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnHero5Clicked);
	}
	
	// Bind Choose Companion button
	if (UButton* CompBtn = FindButtonInMenu(TEXT("Btn_ChooseCompanion")))
	{
		CompBtn->OnClicked.RemoveAll(this);
		CompBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnChooseCompanionClicked);
		UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Bound Choose Companion button"));
	}
	
	// Bind Enter the Tribulation button
	if (UButton* EnterBtn = FindButtonInMenu(TEXT("Btn_EnterTribulation")))
	{
		EnterBtn->OnClicked.RemoveAll(this);
		EnterBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnEnterTribulationClicked);
		UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Bound Enter Tribulation button"));
	}
	
	// Bind arrow buttons for cycling
	if (UButton* PrevBtn = FindButtonInMenu(TEXT("Btn_PrevHero")))
	{
		PrevBtn->OnClicked.RemoveAll(this);
		PrevBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnPrevHeroClicked);
		UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Bound Prev Hero button"));
	}
	if (UButton* NextBtn = FindButtonInMenu(TEXT("Btn_NextHero")))
	{
		NextBtn->OnClicked.RemoveAll(this);
		NextBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnNextHeroClicked);
		UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Bound Next Hero button"));
	}
	
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Hero selection buttons bound"));
}

void UT66UIShellSubsystem::OnHero0Clicked() { SelectHero(0); }
void UT66UIShellSubsystem::OnHero1Clicked() { SelectHero(1); }
void UT66UIShellSubsystem::OnHero2Clicked() { SelectHero(2); }
void UT66UIShellSubsystem::OnHero3Clicked() { SelectHero(3); }
void UT66UIShellSubsystem::OnHero4Clicked() { SelectHero(4); }
void UT66UIShellSubsystem::OnHero5Clicked() { SelectHero(5); }

void UT66UIShellSubsystem::OnPrevHeroClicked()
{
	UGameInstance* GI = GetGameInstance();
	if (UT66SelectionSubsystem* SelectionSS = GI ? GI->GetSubsystem<UT66SelectionSubsystem>() : nullptr)
	{
		int32 CurrentIndex = SelectionSS->GetSelectedHeroIndex();
		int32 NewIndex = (CurrentIndex - 1 + SelectionSS->GetHeroCount()) % SelectionSS->GetHeroCount();
		SelectHero(NewIndex);
	}
}

void UT66UIShellSubsystem::OnNextHeroClicked()
{
	UGameInstance* GI = GetGameInstance();
	if (UT66SelectionSubsystem* SelectionSS = GI ? GI->GetSubsystem<UT66SelectionSubsystem>() : nullptr)
	{
		int32 CurrentIndex = SelectionSS->GetSelectedHeroIndex();
		int32 NewIndex = (CurrentIndex + 1) % SelectionSS->GetHeroCount();
		SelectHero(NewIndex);
	}
}

void UT66UIShellSubsystem::SelectHero(int32 Index)
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Hero %d selected"), Index);
	
	UGameInstance* GI = GetGameInstance();
	if (UT66SelectionSubsystem* SelectionSS = GI ? GI->GetSubsystem<UT66SelectionSubsystem>() : nullptr)
	{
		SelectionSS->SelectHero(Index);
		UpdateHeroDisplay();
	}
}

void UT66UIShellSubsystem::UpdateHeroDisplay()
{
	// Update the hero name, description, and preview
	UGameInstance* GI = GetGameInstance();
	UT66SelectionSubsystem* SelectionSS = GI ? GI->GetSubsystem<UT66SelectionSubsystem>() : nullptr;
	if (!SelectionSS || !CurrentScreen) return;
	
	const FT66HeroData& Hero = SelectionSS->GetSelectedHero();
	
	// Update 3D preview via preview subsystem
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		if (UT66PreviewSubsystem* PreviewSS = World->GetSubsystem<UT66PreviewSubsystem>())
		{
			PreviewSS->ShowHeroPreview(Hero.BodyColor);
			
			// Update image widget with render target
			TArray<UWidget*> AllWidgets;
			CurrentScreen->WidgetTree->GetAllWidgets(AllWidgets);
			
			for (UWidget* Widget : AllWidgets)
			{
				if (UImage* Image = Cast<UImage>(Widget))
				{
					if (Widget->GetName().Contains(TEXT("Img_HeroPreview")))
					{
						if (UTextureRenderTarget2D* RT = PreviewSS->GetHeroRenderTarget())
						{
							Image->SetBrushResourceObject(RT);
							Image->SetVisibility(ESlateVisibility::Visible);
						}
					}
				}
			}
		}
	}
	
	// Find and update text widgets
	TArray<UWidget*> AllWidgets;
	CurrentScreen->WidgetTree->GetAllWidgets(AllWidgets);
	
	for (UWidget* Widget : AllWidgets)
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			if (Widget->GetName().Contains(TEXT("HeroName")))
			{
				TextBlock->SetText(FText::FromString(Hero.DisplayName.ToUpper()));
				TextBlock->SetColorAndOpacity(FSlateColor(Hero.BodyColor));
			}
			else if (Widget->GetName().Contains(TEXT("HeroDesc")))
			{
				TextBlock->SetText(FText::FromString(Hero.Description));
			}
		}
		// Update fallback preview cylinder color
		else if (UBorder* Border = Cast<UBorder>(Widget))
		{
			if (Widget->GetName().Contains(TEXT("HeroCylinder")))
			{
				Border->SetBrushColor(Hero.BodyColor);
			}
		}
	}
	
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Updated display for hero: %s"), *Hero.DisplayName);
}

void UT66UIShellSubsystem::OnChooseCompanionClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Choose Companion clicked -> Companion Selection"));
	
	// Don't close hero selection, spawn companion selection on top
	SpawnScreen(SCREEN_COMPANIONSELECT, 110);
	
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		World->GetTimerManager().SetTimerForNextTick([this]()
		{
			BindCompanionSelectionButtons();
			BindBackButton();
		});
	}
}

void UT66UIShellSubsystem::OnEnterTribulationClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Enter The Tribulation clicked -> Starting gameplay"));
	StartGameplay();
}

void UT66UIShellSubsystem::BindCompanionSelectionButtons()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Binding companion selection buttons..."));
	
	// Bind No Companion button
	if (UButton* NoCompBtn = FindButtonInMenu(TEXT("Btn_NoCompanion")))
	{
		NoCompBtn->OnClicked.RemoveAll(this);
		NoCompBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnNoCompanionClicked);
	}
	
	// Bind individual companion buttons
	if (UButton* Btn = FindButtonInMenu(TEXT("Btn_Companion_0")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnCompanion0Clicked);
	}
	if (UButton* Btn = FindButtonInMenu(TEXT("Btn_Companion_1")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnCompanion1Clicked);
	}
	if (UButton* Btn = FindButtonInMenu(TEXT("Btn_Companion_2")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnCompanion2Clicked);
	}
	if (UButton* Btn = FindButtonInMenu(TEXT("Btn_Companion_3")))
	{
		Btn->OnClicked.RemoveAll(this);
		Btn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnCompanion3Clicked);
	}
	
	// Bind Confirm Companion button
	if (UButton* ConfirmBtn = FindButtonInMenu(TEXT("Btn_ConfirmCompanion")))
	{
		ConfirmBtn->OnClicked.RemoveAll(this);
		ConfirmBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnConfirmCompanionClicked);
	}
	
	// Bind arrow buttons for cycling
	if (UButton* PrevBtn = FindButtonInMenu(TEXT("Btn_PrevCompanion")))
	{
		PrevBtn->OnClicked.RemoveAll(this);
		PrevBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnPrevCompanionClicked);
		UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Bound Prev Companion button"));
	}
	if (UButton* NextBtn = FindButtonInMenu(TEXT("Btn_NextCompanion")))
	{
		NextBtn->OnClicked.RemoveAll(this);
		NextBtn->OnClicked.AddDynamic(this, &UT66UIShellSubsystem::OnNextCompanionClicked);
		UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Bound Next Companion button"));
	}
	
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Companion selection buttons bound"));
}

void UT66UIShellSubsystem::OnCompanion0Clicked() { SelectCompanion(0); }
void UT66UIShellSubsystem::OnCompanion1Clicked() { SelectCompanion(1); }
void UT66UIShellSubsystem::OnCompanion2Clicked() { SelectCompanion(2); }
void UT66UIShellSubsystem::OnCompanion3Clicked() { SelectCompanion(3); }

void UT66UIShellSubsystem::OnPrevCompanionClicked()
{
	UGameInstance* GI = GetGameInstance();
	if (UT66SelectionSubsystem* SelectionSS = GI ? GI->GetSubsystem<UT66SelectionSubsystem>() : nullptr)
	{
		int32 CurrentIndex = SelectionSS->GetSelectedCompanionIndex();
		// -1 means no companion, so wrap from 0 to last companion
		if (CurrentIndex < 0) CurrentIndex = 0;
		int32 NewIndex = (CurrentIndex - 1 + SelectionSS->GetCompanionCount()) % SelectionSS->GetCompanionCount();
		SelectCompanion(NewIndex);
	}
}

void UT66UIShellSubsystem::OnNextCompanionClicked()
{
	UGameInstance* GI = GetGameInstance();
	if (UT66SelectionSubsystem* SelectionSS = GI ? GI->GetSubsystem<UT66SelectionSubsystem>() : nullptr)
	{
		int32 CurrentIndex = SelectionSS->GetSelectedCompanionIndex();
		if (CurrentIndex < 0) CurrentIndex = -1;
		int32 NewIndex = (CurrentIndex + 1) % SelectionSS->GetCompanionCount();
		SelectCompanion(NewIndex);
	}
}

void UT66UIShellSubsystem::SelectCompanion(int32 Index)
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Companion %d selected"), Index);
	
	UGameInstance* GI = GetGameInstance();
	if (UT66SelectionSubsystem* SelectionSS = GI ? GI->GetSubsystem<UT66SelectionSubsystem>() : nullptr)
	{
		SelectionSS->SelectCompanion(Index);
		UpdateCompanionDisplay();
	}
}

void UT66UIShellSubsystem::UpdateCompanionDisplay()
{
	// Update the companion name, description, and preview
	UGameInstance* GI = GetGameInstance();
	UT66SelectionSubsystem* SelectionSS = GI ? GI->GetSubsystem<UT66SelectionSubsystem>() : nullptr;
	if (!SelectionSS || !CurrentScreen) return;
	
	const FT66CompanionData& Companion = SelectionSS->GetSelectedCompanion();
	
	// Update 3D preview via preview subsystem
	UWorld* World = CurrentWorld.Get();
	if (World)
	{
		if (UT66PreviewSubsystem* PreviewSS = World->GetSubsystem<UT66PreviewSubsystem>())
		{
			PreviewSS->ShowCompanionPreview(Companion.BodyColor);
			
			// Update image widget with render target
			TArray<UWidget*> AllWidgets;
			CurrentScreen->WidgetTree->GetAllWidgets(AllWidgets);
			
			for (UWidget* Widget : AllWidgets)
			{
				if (UImage* Image = Cast<UImage>(Widget))
				{
					if (Widget->GetName().Contains(TEXT("Img_CompanionPreview")))
					{
						if (UTextureRenderTarget2D* RT = PreviewSS->GetCompanionRenderTarget())
						{
							Image->SetBrushResourceObject(RT);
							Image->SetVisibility(ESlateVisibility::Visible);
						}
					}
				}
			}
		}
	}
	
	// Find and update text widgets
	TArray<UWidget*> AllWidgets;
	CurrentScreen->WidgetTree->GetAllWidgets(AllWidgets);
	
	for (UWidget* Widget : AllWidgets)
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			if (Widget->GetName().Contains(TEXT("CompanionName")))
			{
				TextBlock->SetText(FText::FromString(Companion.DisplayName.ToUpper()));
				TextBlock->SetColorAndOpacity(FSlateColor(Companion.BodyColor));
			}
			else if (Widget->GetName().Contains(TEXT("CompanionLore")))
			{
				TextBlock->SetText(FText::FromString(Companion.Description));
			}
		}
		// Update fallback preview cube color
		else if (UBorder* Border = Cast<UBorder>(Widget))
		{
			if (Widget->GetName().Contains(TEXT("CompanionCube")))
			{
				Border->SetBrushColor(Companion.BodyColor);
			}
		}
	}
	
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Updated display for companion: %s"), *Companion.DisplayName);
}

void UT66UIShellSubsystem::OnNoCompanionClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] No Companion clicked"));
	
	UGameInstance* GI = GetGameInstance();
	if (UT66SelectionSubsystem* SelectionSS = GI ? GI->GetSubsystem<UT66SelectionSubsystem>() : nullptr)
	{
		SelectionSS->SelectNoCompanion();
	}
}

void UT66UIShellSubsystem::OnConfirmCompanionClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[T66UIShellSubsystem] Confirm Companion clicked -> Back to Hero Selection"));
	
	// Close companion selection and return to hero selection
	CloseCurrentScreen();
}
