#include "T66WidgetLayoutRecipes.h"

#include "GameplayTagContainer.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Border.h"

#include "UObject/UnrealType.h"

// ✅ Shared button component classes
static const TCHAR* T66_ACTION_BUTTON_CLASS_PATH =
TEXT("/Game/Tribulation66/Content/UI/Components/Button/WBP_Comp_Button_Action.WBP_Comp_Button_Action_C");

static const TCHAR* T66_ICON_BUTTON_CLASS_PATH =
TEXT("/Game/Tribulation66/Content/UI/Components/Button/WBP_Comp_Button_IconOnly.WBP_Comp_Button_IconOnly_C");

// ✅ Exposed variables on WBP_Comp_Button_Action (and optionally on other button widgets if they match)
static const FName VAR_ControlID(TEXT("ControlID"));
static const FName VAR_ActionTag(TEXT("ActionTag"));
static const FName VAR_RouteTag(TEXT("RouteTag"));
static const FName VAR_LabelText(TEXT("LabelText"));

namespace T66WidgetLayoutRecipes_Internal
{
	// ------------------------------------------------------------
	// Utility: Tags + property setting (repair/add-only)
	// ------------------------------------------------------------

	static FGameplayTag SafeTag(const FString& TagString)
	{
		const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString), /*ErrorIfNotFound=*/false);
		if (!Tag.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] Missing GameplayTag: %s"), *TagString);
		}
		return Tag;
	}

	static bool TrySetGameplayTagProperty_IfEmpty(UObject* Obj, const FName PropName, const FGameplayTag& NewValue)
	{
		if (!Obj || !NewValue.IsValid())
		{
			return false;
		}

		FStructProperty* StructProp = FindFProperty<FStructProperty>(Obj->GetClass(), PropName);
		if (!StructProp || StructProp->Struct != FGameplayTag::StaticStruct())
		{
			return false; // Property doesn't exist or isn't a GameplayTag (safe no-op)
		}

		void* ValuePtr = StructProp->ContainerPtrToValuePtr<void>(Obj);
		if (!ValuePtr)
		{
			return false;
		}

		FGameplayTag& Current = *reinterpret_cast<FGameplayTag*>(ValuePtr);

		// ✅ Add/Repair only: do not overwrite an existing valid value
		if (Current.IsValid())
		{
			return false;
		}

		Current = NewValue;
		return true;
	}

	static bool TrySetTextProperty_IfEmpty(UObject* Obj, const FName PropName, const FText& NewValue)
	{
		if (!Obj)
		{
			return false;
		}

		FTextProperty* TextProp = FindFProperty<FTextProperty>(Obj->GetClass(), PropName);
		if (!TextProp)
		{
			return false; // Property doesn't exist (safe no-op)
		}

		void* ValuePtr = TextProp->ContainerPtrToValuePtr<void>(Obj);
		if (!ValuePtr)
		{
			return false;
		}

		FText& Current = *reinterpret_cast<FText*>(ValuePtr);

		// ✅ Add/Repair only: do not overwrite existing non-empty text
		if (!Current.IsEmpty())
		{
			return false;
		}

		Current = NewValue;
		return true;
	}

	static bool VerticalBoxHasChild(UVerticalBox* VB, UWidget* Child)
	{
		if (!VB || !Child)
		{
			return false;
		}

		const int32 Num = VB->GetChildrenCount();
		for (int32 i = 0; i < Num; i++)
		{
			if (VB->GetChildAt(i) == Child)
			{
				return true;
			}
		}
		return false;
	}

	static bool HorizontalBoxHasChild(UHorizontalBox* HB, UWidget* Child)
	{
		if (!HB || !Child)
		{
			return false;
		}

		const int32 Num = HB->GetChildrenCount();
		for (int32 i = 0; i < Num; i++)
		{
			if (HB->GetChildAt(i) == Child)
			{
				return true;
			}
		}
		return false;
	}

	static UClass* LoadActionButtonClass()
	{
		UClass* Cls = StaticLoadClass(UUserWidget::StaticClass(), nullptr, T66_ACTION_BUTTON_CLASS_PATH);
		if (!Cls)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66WidgetLayoutRecipes] Failed to load Action Button class: %s"), T66_ACTION_BUTTON_CLASS_PATH);
		}
		return Cls;
	}

	static UClass* LoadIconButtonClass()
	{
		UClass* Cls = StaticLoadClass(UUserWidget::StaticClass(), nullptr, T66_ICON_BUTTON_CLASS_PATH);
		if (!Cls)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] Failed to load IconOnly Button class: %s (will fallback to Action button)"), T66_ICON_BUTTON_CLASS_PATH);
		}
		return Cls;
	}

	static UWidget* EnsureUserWidgetInVerticalBox(
		UWidgetBlueprint* BP,
		UVerticalBox* VB,
		UClass* WidgetClass,
		const FName WidgetName)
	{
		if (!BP || !BP->WidgetTree || !VB || !WidgetClass)
		{
			return nullptr;
		}

		if (UWidget* Existing = BP->WidgetTree->FindWidget(WidgetName))
		{
			return Existing;
		}

		UWidget* NewWidget = BP->WidgetTree->ConstructWidget<UWidget>(WidgetClass, WidgetName);
		if (!NewWidget)
		{
			return nullptr;
		}

		if (!VerticalBoxHasChild(VB, NewWidget))
		{
			VB->AddChildToVerticalBox(NewWidget);
		}

		return NewWidget;
	}

	static UWidget* EnsureUserWidgetInHorizontalBox(
		UWidgetBlueprint* BP,
		UHorizontalBox* HB,
		UClass* WidgetClass,
		const FName WidgetName)
	{
		if (!BP || !BP->WidgetTree || !HB || !WidgetClass)
		{
			return nullptr;
		}

		if (UWidget* Existing = BP->WidgetTree->FindWidget(WidgetName))
		{
			return Existing;
		}

		UWidget* NewWidget = BP->WidgetTree->ConstructWidget<UWidget>(WidgetClass, WidgetName);
		if (!NewWidget)
		{
			return nullptr;
		}

		if (!HorizontalBoxHasChild(HB, NewWidget))
		{
			HB->AddChildToHorizontalBox(NewWidget);
		}

		return NewWidget;
	}

	static UWidget* EnsureActionButton(
		UWidgetBlueprint* BP,
		UVerticalBox* VB,
		UClass* ButtonClass,
		const FName WidgetName,
		const FText& Label,
		const FGameplayTag& ControlID,
		const FGameplayTag& ActionTag,
		const FGameplayTag& RouteTag)
	{
		UWidget* Btn = EnsureUserWidgetInVerticalBox(BP, VB, ButtonClass, WidgetName);
		if (!Btn)
		{
			return nullptr;
		}

		UObject* Obj = Btn;
		TrySetGameplayTagProperty_IfEmpty(Obj, VAR_ControlID, ControlID);
		TrySetGameplayTagProperty_IfEmpty(Obj, VAR_ActionTag, ActionTag);
		TrySetGameplayTagProperty_IfEmpty(Obj, VAR_RouteTag, RouteTag);
		TrySetTextProperty_IfEmpty(Obj, VAR_LabelText, Label);

		return Btn;
	}

	static UWidget* EnsureIconButton(
		UWidgetBlueprint* BP,
		UHorizontalBox* HB,
		UClass* IconButtonClass,
		const FName WidgetName,
		const FGameplayTag& ControlID,
		const FGameplayTag& ActionTag,
		const FGameplayTag& RouteTag)
	{
		UWidget* Btn = EnsureUserWidgetInHorizontalBox(BP, HB, IconButtonClass, WidgetName);
		if (!Btn)
		{
			return nullptr;
		}

		// We only set properties if they exist (safe for IconOnly)
		UObject* Obj = Btn;
		TrySetGameplayTagProperty_IfEmpty(Obj, VAR_ControlID, ControlID);
		TrySetGameplayTagProperty_IfEmpty(Obj, VAR_ActionTag, ActionTag);
		TrySetGameplayTagProperty_IfEmpty(Obj, VAR_RouteTag, RouteTag);

		// DO NOT set LabelText on icon buttons (they should remain icon-only)
		return Btn;
	}

	// ------------------------------------------------------------
	// Root safety helpers
	// ------------------------------------------------------------

	static UCanvasPanel* EnsureRootCanvas_IfEmpty(UWidgetBlueprint* BP, const FName RootName)
	{
		if (!BP || !BP->WidgetTree)
		{
			return nullptr;
		}

		// ✅ If a root already exists, never replace it
		if (BP->WidgetTree->RootWidget)
		{
			return Cast<UCanvasPanel>(BP->WidgetTree->RootWidget);
		}

		UCanvasPanel* Canvas = BP->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), RootName);
		if (!Canvas)
		{
			return nullptr;
		}

		BP->WidgetTree->RootWidget = Canvas;
		return Canvas;
	}

	static bool RootCanvasIsInjectable(UCanvasPanel* RootCanvas)
	{
		return RootCanvas && (RootCanvas->GetChildrenCount() == 0);
	}

	static UVerticalBox* InjectCenteredVerticalBox(UWidgetBlueprint* BP, UCanvasPanel* RootCanvas, const FName VBName, const FVector2D& Size)
	{
		if (!BP || !BP->WidgetTree || !RootCanvas)
		{
			return nullptr;
		}

		if (UWidget* Found = BP->WidgetTree->FindWidget(VBName))
		{
			return Cast<UVerticalBox>(Found);
		}

		if (!RootCanvasIsInjectable(RootCanvas))
		{
			return nullptr;
		}

		UVerticalBox* VB = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), VBName);
		if (!VB)
		{
			return nullptr;
		}

		RootCanvas->AddChild(VB);

		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(VB->Slot))
		{
			Slot->SetAnchors(FAnchors(0.5f, 0.5f));
			Slot->SetAlignment(FVector2D(0.5f, 0.5f));
			Slot->SetPosition(FVector2D(0.f, 0.f));
			Slot->SetSize(Size);
			Slot->SetAutoSize(false);
		}

		return VB;
	}

	static UBorder* InjectFullBorder(UWidgetBlueprint* BP, UCanvasPanel* RootCanvas, const FName BorderName)
	{
		if (!BP || !BP->WidgetTree || !RootCanvas)
		{
			return nullptr;
		}

		if (UWidget* Found = BP->WidgetTree->FindWidget(BorderName))
		{
			return Cast<UBorder>(Found);
		}

		if (!RootCanvasIsInjectable(RootCanvas))
		{
			return nullptr;
		}

		UBorder* Border = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), BorderName);
		if (!Border)
		{
			return nullptr;
		}

		RootCanvas->AddChild(Border);

		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Border->Slot))
		{
			Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			Slot->SetOffsets(FMargin(0.f));
			Slot->SetAlignment(FVector2D(0.f, 0.f));
		}

		return Border;
	}

	// ------------------------------------------------------------
	// Recipe: BootIntro (auto)
	// ------------------------------------------------------------

	static bool ApplyRecipe_BootIntro(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_BootIntro"))
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] BootIntro root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UBorder* Border = InjectFullBorder(BP, Root, FName(TEXT("Panel_BootIntro")));
		if (!Border)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] BootIntro already has layout. Skipping injection."));
			return false;
		}

		return true;
	}

	// ------------------------------------------------------------
	// Recipe: MainMenu
	// ------------------------------------------------------------

	static bool ApplyRecipe_MainMenu(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_MainMenu"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		UClass* IconButtonClass = LoadIconButtonClass();
		if (!IconButtonClass)
		{
			IconButtonClass = ActionButtonClass;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] MainMenu root exists but isn't a CanvasPanel. Skipping for safety."));
			return false;
		}

		if (!RootCanvasIsInjectable(Root))
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] MainMenu already has layout. Skipping injection for safety."));
			return false;
		}

		UHorizontalBox* HB = BP->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_MainMenu")));
		Root->AddChild(HB);

		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(HB->Slot))
		{
			Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			Slot->SetOffsets(FMargin(0.f));
			Slot->SetAlignment(FVector2D(0.f, 0.f));
		}

		UBorder* PanelLeft = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_LeftMenu")));
		HB->AddChildToHorizontalBox(PanelLeft);

		UBorder* PanelRight = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_RightLeaderboard")));
		HB->AddChildToHorizontalBox(PanelRight);

		UVerticalBox* VB_Left = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_LeftMenu")));
		PanelLeft->SetContent(VB_Left);

		UVerticalBox* VB_Buttons = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_MainMenuButtons")));
		VB_Left->AddChildToVerticalBox(VB_Buttons);

		UHorizontalBox* HB_Icons = BP->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_MainMenuIcons")));
		VB_Left->AddChildToVerticalBox(HB_Icons);

		const FGameplayTag FocusNavPrimary = SafeTag(TEXT("Focus.Nav.Primary"));

		// Routes
		const FGameplayTag Route_PartySize = SafeTag(TEXT("UI.Screen.PartySizePicker"));
		const FGameplayTag Route_SaveSlots = SafeTag(TEXT("UI.Screen.SaveSlots"));
		const FGameplayTag Route_Achievements = SafeTag(TEXT("UI.Screen.Achievements"));
		const FGameplayTag Route_AccountStatus = SafeTag(TEXT("UI.Screen.AccountStatus"));

		// Modals
		const FGameplayTag Route_SettingsModal = SafeTag(TEXT("UI.Modal.Settings"));
		const FGameplayTag Route_LanguageModal = SafeTag(TEXT("UI.Modal.Language"));
		const FGameplayTag Route_QuitConfirm = SafeTag(TEXT("UI.Modal.QuitConfirm"));

		// Actions
		const FGameplayTag Action_StartNew = SafeTag(TEXT("UI.Action.StartNewGame"));
		const FGameplayTag Action_LoadGame = SafeTag(TEXT("UI.Action.LoadGame"));
		const FGameplayTag Action_Quit = SafeTag(TEXT("UI.Action.QuitGame"));

		EnsureActionButton(BP, VB_Buttons, ActionButtonClass, FName(TEXT("Button_NewGame")),
			FText::FromString(TEXT("New Game")),
			FocusNavPrimary,
			Action_StartNew,
			Route_PartySize);

		EnsureActionButton(BP, VB_Buttons, ActionButtonClass, FName(TEXT("Button_LoadGame")),
			FText::FromString(TEXT("Load Game")),
			FGameplayTag(),
			Action_LoadGame,
			Route_SaveSlots);

		EnsureActionButton(BP, VB_Buttons, ActionButtonClass, FName(TEXT("Button_Settings")),
			FText::FromString(TEXT("Settings")),
			FGameplayTag(),
			FGameplayTag(),
			Route_SettingsModal);

		EnsureActionButton(BP, VB_Buttons, ActionButtonClass, FName(TEXT("Button_Achievements")),
			FText::FromString(TEXT("Achievements")),
			FGameplayTag(),
			FGameplayTag(),
			Route_Achievements);

		// Optional (safe to stamp; you can hide it later)
		EnsureActionButton(BP, VB_Buttons, ActionButtonClass, FName(TEXT("Button_AccountStatus")),
			FText::FromString(TEXT("Account Status")),
			FGameplayTag(),
			FGameplayTag(),
			Route_AccountStatus);

		EnsureIconButton(BP, HB_Icons, IconButtonClass, FName(TEXT("Button_LanguageIcon")),
			FGameplayTag(),
			FGameplayTag(),
			Route_LanguageModal);

		EnsureIconButton(BP, HB_Icons, IconButtonClass, FName(TEXT("Button_QuitIcon")),
			FGameplayTag(),
			Action_Quit,
			Route_QuitConfirm);

		return true;
	}

	// ------------------------------------------------------------
	// Recipe: PartySizePicker
	// ------------------------------------------------------------

	static bool ApplyRecipe_PartySizePicker(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_PartySizePicker"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] PartySizePicker root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_PartySizePicker")), FVector2D(600.f, 320.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] PartySizePicker already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
		const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));

		const FGameplayTag Route_Solo = SafeTag(TEXT("UI.Screen.HeroSelect_Solo"));
		const FGameplayTag Route_Coop = SafeTag(TEXT("UI.Screen.HeroSelect_Coop"));
		const FGameplayTag Route_Back = SafeTag(TEXT("UI.Screen.MainMenu"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Solo")),
			FText::FromString(TEXT("Solo")),
			FocusPrimary,
			ActionConfirm,
			Route_Solo);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Coop")),
			FText::FromString(TEXT("Co-op")),
			FGameplayTag(),
			ActionConfirm,
			Route_Coop);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Back")),
			FText::FromString(TEXT("Back")),
			FGameplayTag(),
			ActionBack,
			Route_Back);

		return true;
	}

	// ------------------------------------------------------------
	// Recipe: Generic List Screen helper
	// ------------------------------------------------------------

	static bool ApplySimpleListScreen(
		UWidgetBlueprint* BP,
		const FString& ExpectedName,
		const FName EntryName,
		const FText& EntryLabel,
		const FGameplayTag& EntryFocusTag,
		const FName BackButtonName,
		const FGameplayTag& BackRoute)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != ExpectedName)
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] %s root exists but isn't a CanvasPanel. Skipping."), *ExpectedName);
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_Root")), FVector2D(800.f, 600.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] %s already has layout. Skipping injection."), *ExpectedName);
			return false;
		}

		EnsureActionButton(BP, VB, ActionButtonClass, EntryName, EntryLabel, EntryFocusTag, FGameplayTag(), FGameplayTag());
		EnsureActionButton(BP, VB, ActionButtonClass, BackButtonName, FText::FromString(TEXT("Back")), FGameplayTag(), SafeTag(TEXT("UI.Action.Back")), BackRoute);

		return true;
	}

	// ------------------------------------------------------------
	// Recipe: HeroSelect Solo/Coop
	// ------------------------------------------------------------

	static bool ApplyRecipe_HeroSelect_Solo(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_HeroSelect_Solo"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] HeroSelect_Solo root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_HeroSelect_Solo")), FVector2D(900.f, 650.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] HeroSelect_Solo already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusList = SafeTag(TEXT("Focus.List"));
		const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));

		const FGameplayTag Route_CompanionSelect = SafeTag(TEXT("UI.Screen.CompanionSelect"));
		const FGameplayTag Route_Back = SafeTag(TEXT("UI.Screen.PartySizePicker"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Hero_0")),
			FText::FromString(TEXT("Hero 1")),
			FocusList,
			ActionConfirm,
			FGameplayTag());

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Confirm")),
			FText::FromString(TEXT("Confirm")),
			FGameplayTag(),
			ActionConfirm,
			Route_CompanionSelect);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Back")),
			FText::FromString(TEXT("Back")),
			FGameplayTag(),
			ActionBack,
			Route_Back);

		return true;
	}

	static bool ApplyRecipe_HeroSelect_Coop(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_HeroSelect_Coop"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] HeroSelect_Coop root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_HeroSelect_Coop")), FVector2D(900.f, 650.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] HeroSelect_Coop already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusList = SafeTag(TEXT("Focus.List"));
		const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));

		const FGameplayTag Route_CompanionSelect = SafeTag(TEXT("UI.Screen.CompanionSelect"));
		const FGameplayTag Route_Back = SafeTag(TEXT("UI.Screen.PartySizePicker"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Hero_0")),
			FText::FromString(TEXT("Hero 1")),
			FocusList,
			ActionConfirm,
			FGameplayTag());

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Confirm")),
			FText::FromString(TEXT("Confirm")),
			FGameplayTag(),
			ActionConfirm,
			Route_CompanionSelect);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Back")),
			FText::FromString(TEXT("Back")),
			FGameplayTag(),
			ActionBack,
			Route_Back);

		return true;
	}

	// ------------------------------------------------------------
	// Recipe: CompanionSelect
	// ------------------------------------------------------------

	static bool ApplyRecipe_CompanionSelect(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_CompanionSelect"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] CompanionSelect root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_CompanionSelect")), FVector2D(900.f, 650.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] CompanionSelect already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusList = SafeTag(TEXT("Focus.List"));
		const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));

		const FGameplayTag Route_HeroSelectSolo = SafeTag(TEXT("UI.Screen.HeroSelect_Solo"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Companion_0")),
			FText::FromString(TEXT("Companion 1")),
			FocusList,
			ActionConfirm,
			FGameplayTag());

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Confirm")),
			FText::FromString(TEXT("Confirm")),
			FGameplayTag(),
			ActionConfirm,
			Route_HeroSelectSolo);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Back")),
			FText::FromString(TEXT("Back")),
			FGameplayTag(),
			ActionBack,
			Route_HeroSelectSolo);

		return true;
	}

	// ------------------------------------------------------------
	// Recipe: SaveSlots
	// ------------------------------------------------------------

	static bool ApplyRecipe_SaveSlots(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_SaveSlots"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] SaveSlots root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_SaveSlots")), FVector2D(900.f, 650.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] SaveSlots already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusList = SafeTag(TEXT("Focus.List"));
		const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
		const FGameplayTag ActionDelete = SafeTag(TEXT("UI.Action.Delete"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));

		const FGameplayTag Route_SavePreview = SafeTag(TEXT("UI.Screen.SavePreview"));
		const FGameplayTag Route_MainMenu = SafeTag(TEXT("UI.Screen.MainMenu"));
		const FGameplayTag Route_DeleteConfirm = SafeTag(TEXT("UI.Modal.DeleteSaveConfirm"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Slot_0")),
			FText::FromString(TEXT("Slot 1")),
			FocusList,
			ActionConfirm,
			Route_SavePreview);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Delete")),
			FText::FromString(TEXT("Delete")),
			FGameplayTag(),
			ActionDelete,
			Route_DeleteConfirm);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Back")),
			FText::FromString(TEXT("Back")),
			FGameplayTag(),
			ActionBack,
			Route_MainMenu);

		return true;
	}

	// ------------------------------------------------------------
	// Recipe: SavePreview
	// ------------------------------------------------------------

	static bool ApplyRecipe_SavePreview(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_SavePreview"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] SavePreview root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_SavePreview")), FVector2D(900.f, 650.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] SavePreview already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
		const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
		const FGameplayTag ActionDelete = SafeTag(TEXT("UI.Action.Delete"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));

		const FGameplayTag Route_GameStart = SafeTag(TEXT("UI.Screen.Lobby_Load"));
		const FGameplayTag Route_SaveSlots = SafeTag(TEXT("UI.Screen.SaveSlots"));
		const FGameplayTag Route_DeleteConfirm = SafeTag(TEXT("UI.Modal.DeleteSaveConfirm"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Continue")),
			FText::FromString(TEXT("Continue")),
			FocusPrimary,
			ActionConfirm,
			Route_GameStart);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Delete")),
			FText::FromString(TEXT("Delete")),
			FGameplayTag(),
			ActionDelete,
			Route_DeleteConfirm);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Back")),
			FText::FromString(TEXT("Back")),
			FGameplayTag(),
			ActionBack,
			Route_SaveSlots);

		return true;
	}

	// ------------------------------------------------------------
	// Recipe: Lobby_New / Lobby_Load
	// ------------------------------------------------------------

	static bool ApplyRecipe_Lobby_Common(UWidgetBlueprint* BP, const FString& ExpectedName, const FString& StartLabel)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != ExpectedName)
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] %s root exists but isn't a CanvasPanel. Skipping."), *ExpectedName);
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_Lobby")), FVector2D(800.f, 400.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] %s already has layout. Skipping injection."), *ExpectedName);
			return false;
		}

		const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
		const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));

		const FGameplayTag Route_InRun = SafeTag(TEXT("UI.Screen.InRunHUD"));
		const FGameplayTag Route_MainMenu = SafeTag(TEXT("UI.Screen.MainMenu"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Start")),
			FText::FromString(*StartLabel),
			FocusPrimary,
			ActionConfirm,
			Route_InRun);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Back")),
			FText::FromString(TEXT("Back")),
			FGameplayTag(),
			ActionBack,
			Route_MainMenu);

		return true;
	}

	static bool ApplyRecipe_Lobby_New(UWidgetBlueprint* BP)
	{
		return ApplyRecipe_Lobby_Common(BP, TEXT("WBP_Screen_Lobby_New"), TEXT("Start"));
	}

	static bool ApplyRecipe_Lobby_Load(UWidgetBlueprint* BP)
	{
		return ApplyRecipe_Lobby_Common(BP, TEXT("WBP_Screen_Lobby_Load"), TEXT("Continue"));
	}

	// ------------------------------------------------------------
	// Recipe: InRunHUD
	// ------------------------------------------------------------

	static bool ApplyRecipe_InRunHUD(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_InRunHUD"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] InRunHUD root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_InRunHUD_Min")), FVector2D(300.f, 120.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] InRunHUD already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusNavPrimary = SafeTag(TEXT("Focus.Nav.Primary"));
		const FGameplayTag ActionPause = SafeTag(TEXT("UI.Action.Pause"));
		const FGameplayTag Route_PauseMenu = SafeTag(TEXT("UI.Screen.PauseMenu"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Pause")),
			FText::FromString(TEXT("Pause")),
			FocusNavPrimary,
			ActionPause,
			Route_PauseMenu);

		return true;
	}

	// ------------------------------------------------------------
	// Recipe: PauseMenu
	// ------------------------------------------------------------

	static bool ApplyRecipe_PauseMenu(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_PauseMenu"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] PauseMenu root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_PauseMenu")), FVector2D(600.f, 420.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] PauseMenu already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusNavPrimary = SafeTag(TEXT("Focus.Nav.Primary"));

		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));
		const FGameplayTag ActionQuit = SafeTag(TEXT("UI.Action.QuitGame"));

		const FGameplayTag Route_Settings = SafeTag(TEXT("UI.Modal.Settings"));
		const FGameplayTag Route_ReportBug = SafeTag(TEXT("UI.Modal.ReportBug"));
		const FGameplayTag Route_QuitConfirm = SafeTag(TEXT("UI.Modal.QuitConfirm"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Resume")),
			FText::FromString(TEXT("Resume")),
			FocusNavPrimary,
			ActionBack,
			FGameplayTag());

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Settings")),
			FText::FromString(TEXT("Settings")),
			FGameplayTag(),
			FGameplayTag(),
			Route_Settings);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_ReportBug")),
			FText::FromString(TEXT("Report Bug")),
			FGameplayTag(),
			FGameplayTag(),
			Route_ReportBug);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Quit")),
			FText::FromString(TEXT("Quit")),
			FGameplayTag(),
			ActionQuit,
			Route_QuitConfirm);

		return true;
	}

	// ------------------------------------------------------------
	// Recipe: Achievements + AccountStatus + PlayerSummaryPicker + RunDetails + Results
	// ------------------------------------------------------------

	static bool ApplyRecipe_Achievements(UWidgetBlueprint* BP)
	{
		return ApplySimpleListScreen(
			BP,
			TEXT("WBP_Screen_Achievements"),
			FName(TEXT("Button_Achievement_0")),
			FText::FromString(TEXT("Achievement 1")),
			SafeTag(TEXT("Focus.List")),
			FName(TEXT("Button_Back")),
			SafeTag(TEXT("UI.Screen.MainMenu"))
		);
	}

	static bool ApplyRecipe_AccountStatus(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_AccountStatus"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] AccountStatus root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_AccountStatus")), FVector2D(900.f, 650.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] AccountStatus already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusList = SafeTag(TEXT("Focus.List"));
		const FGameplayTag ActionRefresh = SafeTag(TEXT("UI.Action.Refresh"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));
		const FGameplayTag Route_MainMenu = SafeTag(TEXT("UI.Screen.MainMenu"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_FlagReason_0")),
			FText::FromString(TEXT("Flag Reason 1")),
			FocusList,
			FGameplayTag(),
			FGameplayTag());

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Refresh")),
			FText::FromString(TEXT("Refresh")),
			FGameplayTag(),
			ActionRefresh,
			FGameplayTag());

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Back")),
			FText::FromString(TEXT("Back")),
			FGameplayTag(),
			ActionBack,
			Route_MainMenu);

		return true;
	}

	static bool ApplyRecipe_PlayerSummaryPicker(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_PlayerSummaryPicker"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] PlayerSummaryPicker root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_PlayerSummaryPicker")), FVector2D(900.f, 650.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] PlayerSummaryPicker already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusList = SafeTag(TEXT("Focus.List"));
		const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));

		const FGameplayTag Route_RunDetails = SafeTag(TEXT("UI.Screen.RunDetails"));
		const FGameplayTag Route_MainMenu = SafeTag(TEXT("UI.Screen.MainMenu"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Player_0")),
			FText::FromString(TEXT("Player 1")),
			FocusList,
			ActionConfirm,
			FGameplayTag());

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Confirm")),
			FText::FromString(TEXT("Confirm")),
			FGameplayTag(),
			ActionConfirm,
			Route_RunDetails);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Back")),
			FText::FromString(TEXT("Back")),
			FGameplayTag(),
			ActionBack,
			Route_MainMenu);

		return true;
	}

	static bool ApplyRecipe_RunDetails(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_RunDetails"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] RunDetails root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_RunDetails")), FVector2D(900.f, 650.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] RunDetails already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusList = SafeTag(TEXT("Focus.List"));
		const FGameplayTag ActionReportRun = SafeTag(TEXT("UI.Action.ReportRun"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));

		const FGameplayTag Route_ReportRun = SafeTag(TEXT("UI.Modal.ReportRun"));
		const FGameplayTag Route_MainMenu = SafeTag(TEXT("UI.Screen.MainMenu"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_TimelineEvent_0")),
			FText::FromString(TEXT("Event 1")),
			FocusList,
			FGameplayTag(),
			FGameplayTag());

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_ReportRun")),
			FText::FromString(TEXT("Report Run")),
			FGameplayTag(),
			ActionReportRun,
			Route_ReportRun);

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Back")),
			FText::FromString(TEXT("Back")),
			FGameplayTag(),
			ActionBack,
			Route_MainMenu);

		return true;
	}

	static bool ApplyRecipe_Results_ChapterEnd(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Screen_Results_ChapterEnd"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] Results_ChapterEnd root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		UVerticalBox* VB = InjectCenteredVerticalBox(BP, Root, FName(TEXT("VB_ResultsChapterEnd")), FVector2D(900.f, 650.f));
		if (!VB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] Results_ChapterEnd already has layout. Skipping injection."));
			return false;
		}

		const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
		const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
		const FGameplayTag ActionBack = SafeTag(TEXT("UI.Action.Back"));
		const FGameplayTag Route_MainMenu = SafeTag(TEXT("UI.Screen.MainMenu"));

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_Continue")),
			FText::FromString(TEXT("Continue")),
			FocusPrimary,
			ActionConfirm,
			FGameplayTag());

		EnsureActionButton(BP, VB, ActionButtonClass, FName(TEXT("Button_MainMenu")),
			FText::FromString(TEXT("Main Menu")),
			FGameplayTag(),
			ActionBack,
			Route_MainMenu);

		return true;
	}

	// ------------------------------------------------------------
	// ✅ NEW: Minimum layout recipes for Modals / Overlays / Tooltips
	// These follow the same Add/Repair-only rules as Screens:
	// - Only inject when WidgetTree is empty (no RootWidget yet)
	// - Never overwrite existing layouts
	// ------------------------------------------------------------

	// ------------------------------------------------------------
	// Helper: Inject a centered modal window (Backdrop + Window + Body + Buttons)
	// ------------------------------------------------------------
	static bool ApplySimpleModal(
		UWidgetBlueprint* BP,
		const FString& ExpectedName,
		const FName BackdropName,
		const FName WindowName,
		const FName BodyName,
		const FName ButtonsVBName,
		const FVector2D& WindowSize,
		TFunction<void(UWidgetBlueprint*, UVerticalBox*)> StampButtonsFn)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != ExpectedName)
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] %s root exists but isn't a CanvasPanel. Skipping."), *ExpectedName);
			return false;
		}

		if (!RootCanvasIsInjectable(Root))
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] %s already has layout. Skipping injection for safety."), *ExpectedName);
			return false;
		}

		// Backdrop (full-screen)
		UBorder* Backdrop = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), BackdropName);
		Root->AddChild(Backdrop);

		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Backdrop->Slot))
		{
			Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			Slot->SetOffsets(FMargin(0.f));
			Slot->SetAlignment(FVector2D(0.f, 0.f));
		}

		// Window (centered)
		UBorder* Window = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), WindowName);
		Root->AddChild(Window);

		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Window->Slot))
		{
			Slot->SetAnchors(FAnchors(0.5f, 0.5f));
			Slot->SetAlignment(FVector2D(0.5f, 0.5f));
			Slot->SetPosition(FVector2D(0.f, 0.f));
			Slot->SetSize(WindowSize);
			Slot->SetAutoSize(false);
		}

		UVerticalBox* VB_Window = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_ModalRoot")));
		Window->SetContent(VB_Window);

		// Body placeholder
		UBorder* Body = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), BodyName);
		VB_Window->AddChildToVerticalBox(Body);

		// Buttons
		UVerticalBox* VB_Buttons = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), ButtonsVBName);
		VB_Window->AddChildToVerticalBox(VB_Buttons);

		// Stamp buttons (optional)
		if (StampButtonsFn)
		{
			StampButtonsFn(BP, VB_Buttons);
		}

		return true;
	}

	// ------------------------------------------------------------
	// Modals
	// ------------------------------------------------------------

	static bool ApplyRecipe_Modal_GenericBlocker(UWidgetBlueprint* BP)
	{
		return ApplySimpleModal(
			BP,
			TEXT("WBP_Modal_GenericBlocker"),
			FName(TEXT("Panel_Backdrop")),
			FName(TEXT("Panel_ModalWindow")),
			FName(TEXT("Panel_Body")),
			FName(TEXT("VB_Buttons")),
			FVector2D(800.f, 520.f),
			[](UWidgetBlueprint* InBP, UVerticalBox* VB_Buttons)
			{
				const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
				const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
				const FGameplayTag ActionCancel = SafeTag(TEXT("UI.Action.Cancel"));

				EnsureActionButton(InBP, VB_Buttons, LoadActionButtonClass(), FName(TEXT("Button_Confirm")),
					FText::FromString(TEXT("Confirm")),
					FocusPrimary,
					ActionConfirm,
					FGameplayTag());

				EnsureActionButton(InBP, VB_Buttons, LoadActionButtonClass(), FName(TEXT("Button_Cancel")),
					FText::FromString(TEXT("Cancel")),
					FGameplayTag(),
					ActionCancel,
					FGameplayTag());
			});
	}

	static bool ApplyRecipe_Modal_DeleteSaveConfirm(UWidgetBlueprint* BP)
	{
		return ApplySimpleModal(
			BP,
			TEXT("WBP_Modal_DeleteSaveConfirm"),
			FName(TEXT("Panel_Backdrop")),
			FName(TEXT("Panel_ModalWindow")),
			FName(TEXT("Panel_Body")),
			FName(TEXT("VB_Buttons")),
			FVector2D(820.f, 540.f),
			[](UWidgetBlueprint* InBP, UVerticalBox* VB_Buttons)
			{
				const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
				const FGameplayTag ActionConfirm = SafeTag(TEXT("UI.Action.Confirm"));
				const FGameplayTag ActionCancel = SafeTag(TEXT("UI.Action.Cancel"));

				EnsureActionButton(InBP, VB_Buttons, LoadActionButtonClass(), FName(TEXT("Button_DeleteConfirm")),
					FText::FromString(TEXT("Delete")),
					FocusPrimary,
					ActionConfirm,
					FGameplayTag());

				EnsureActionButton(InBP, VB_Buttons, LoadActionButtonClass(), FName(TEXT("Button_Cancel")),
					FText::FromString(TEXT("Cancel")),
					FGameplayTag(),
					ActionCancel,
					FGameplayTag());
			});
	}

	static bool ApplyRecipe_Modal_ReportRun(UWidgetBlueprint* BP)
	{
		return ApplySimpleModal(
			BP,
			TEXT("WBP_Modal_ReportRun"),
			FName(TEXT("Panel_Backdrop")),
			FName(TEXT("Panel_ModalWindow")),
			FName(TEXT("Panel_Form")),
			FName(TEXT("VB_Buttons")),
			FVector2D(900.f, 650.f),
			[](UWidgetBlueprint* InBP, UVerticalBox* VB_Buttons)
			{
				const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
				const FGameplayTag ActionSubmit = SafeTag(TEXT("UI.Action.Submit"));
				const FGameplayTag ActionCancel = SafeTag(TEXT("UI.Action.Cancel"));

				EnsureActionButton(InBP, VB_Buttons, LoadActionButtonClass(), FName(TEXT("Button_Submit")),
					FText::FromString(TEXT("Submit")),
					FocusPrimary,
					ActionSubmit,
					FGameplayTag());

				EnsureActionButton(InBP, VB_Buttons, LoadActionButtonClass(), FName(TEXT("Button_Cancel")),
					FText::FromString(TEXT("Cancel")),
					FGameplayTag(),
					ActionCancel,
					FGameplayTag());
			});
	}

	static bool ApplyRecipe_Modal_ReportBug(UWidgetBlueprint* BP)
	{
		return ApplySimpleModal(
			BP,
			TEXT("WBP_Modal_ReportBug"),
			FName(TEXT("Panel_Backdrop")),
			FName(TEXT("Panel_ModalWindow")),
			FName(TEXT("Panel_Form")),
			FName(TEXT("VB_Buttons")),
			FVector2D(900.f, 650.f),
			[](UWidgetBlueprint* InBP, UVerticalBox* VB_Buttons)
			{
				const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
				const FGameplayTag ActionSubmit = SafeTag(TEXT("UI.Action.Submit"));
				const FGameplayTag ActionCancel = SafeTag(TEXT("UI.Action.Cancel"));

				EnsureActionButton(InBP, VB_Buttons, LoadActionButtonClass(), FName(TEXT("Button_Submit")),
					FText::FromString(TEXT("Submit")),
					FocusPrimary,
					ActionSubmit,
					FGameplayTag());

				EnsureActionButton(InBP, VB_Buttons, LoadActionButtonClass(), FName(TEXT("Button_Cancel")),
					FText::FromString(TEXT("Cancel")),
					FGameplayTag(),
					ActionCancel,
					FGameplayTag());
			});
	}

	static bool ApplyRecipe_Modal_QuitConfirm(UWidgetBlueprint* BP)
	{
		return ApplySimpleModal(
			BP,
			TEXT("WBP_Modal_QuitConfirm"),
			FName(TEXT("Panel_Backdrop")),
			FName(TEXT("Panel_ModalWindow")),
			FName(TEXT("Panel_Body")),
			FName(TEXT("VB_Buttons")),
			FVector2D(820.f, 520.f),
			[](UWidgetBlueprint* InBP, UVerticalBox* VB_Buttons)
			{
				const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
				const FGameplayTag ActionQuit = SafeTag(TEXT("UI.Action.QuitGame"));
				const FGameplayTag ActionCancel = SafeTag(TEXT("UI.Action.Cancel"));

				EnsureActionButton(InBP, VB_Buttons, LoadActionButtonClass(), FName(TEXT("Button_Quit")),
					FText::FromString(TEXT("Quit Game")),
					FocusPrimary,
					ActionQuit,
					FGameplayTag());

				EnsureActionButton(InBP, VB_Buttons, LoadActionButtonClass(), FName(TEXT("Button_Cancel")),
					FText::FromString(TEXT("Cancel")),
					FGameplayTag(),
					ActionCancel,
					FGameplayTag());
			});
	}

	static bool ApplyRecipe_Modal_Language(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Modal_Language"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] Language root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		if (!RootCanvasIsInjectable(Root))
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] Language already has layout. Skipping injection for safety."));
			return false;
		}

		// Backdrop
		UBorder* Backdrop = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_Backdrop")));
		Root->AddChild(Backdrop);
		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Backdrop->Slot))
		{
			Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			Slot->SetOffsets(FMargin(0.f));
		}

		// Window
		UBorder* Window = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_ModalWindow")));
		Root->AddChild(Window);
		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Window->Slot))
		{
			Slot->SetAnchors(FAnchors(0.5f, 0.5f));
			Slot->SetAlignment(FVector2D(0.5f, 0.5f));
			Slot->SetPosition(FVector2D(0.f, 0.f));
			Slot->SetSize(FVector2D(900.f, 650.f));
			Slot->SetAutoSize(false);
		}

		UVerticalBox* VB_Window = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_LanguageRoot")));
		Window->SetContent(VB_Window);

		// List placeholder
		UBorder* Panel_List = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_LanguageList")));
		VB_Window->AddChildToVerticalBox(Panel_List);

		UVerticalBox* VB_List = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_LanguageEntries")));
		Panel_List->SetContent(VB_List);

		// One starter entry (gives Focus.List something to land on)
		const FGameplayTag FocusList = SafeTag(TEXT("Focus.List"));
		const FGameplayTag ActionToggle = SafeTag(TEXT("UI.Action.Toggle"));
		EnsureActionButton(BP, VB_List, ActionButtonClass, FName(TEXT("Button_Language_0")),
			FText::FromString(TEXT("Language 1")),
			FocusList,
			ActionToggle,
			FGameplayTag());

		// Buttons
		UVerticalBox* VB_Buttons = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Buttons")));
		VB_Window->AddChildToVerticalBox(VB_Buttons);

		const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
		const FGameplayTag ActionApply = SafeTag(TEXT("UI.Action.Apply"));
		const FGameplayTag ActionCancel = SafeTag(TEXT("UI.Action.Cancel"));

		EnsureActionButton(BP, VB_Buttons, ActionButtonClass, FName(TEXT("Button_Apply")),
			FText::FromString(TEXT("Apply")),
			FocusPrimary,
			ActionApply,
			FGameplayTag());

		EnsureActionButton(BP, VB_Buttons, ActionButtonClass, FName(TEXT("Button_Cancel")),
			FText::FromString(TEXT("Cancel")),
			FGameplayTag(),
			ActionCancel,
			FGameplayTag());

		return true;
	}

	static bool ApplyRecipe_Modal_Settings(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != TEXT("WBP_Modal_Settings"))
		{
			return false;
		}

		UClass* ActionButtonClass = LoadActionButtonClass();
		if (!ActionButtonClass)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] Settings root exists but isn't a CanvasPanel. Skipping."));
			return false;
		}

		if (!RootCanvasIsInjectable(Root))
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] Settings already has layout. Skipping injection for safety."));
			return false;
		}

		// Backdrop
		UBorder* Backdrop = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_Backdrop")));
		Root->AddChild(Backdrop);
		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Backdrop->Slot))
		{
			Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			Slot->SetOffsets(FMargin(0.f));
		}

		// Window
		UBorder* Window = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_SettingsWindow")));
		Root->AddChild(Window);
		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Window->Slot))
		{
			Slot->SetAnchors(FAnchors(0.5f, 0.5f));
			Slot->SetAlignment(FVector2D(0.5f, 0.5f));
			Slot->SetPosition(FVector2D(0.f, 0.f));
			Slot->SetSize(FVector2D(1000.f, 700.f));
			Slot->SetAutoSize(false);
		}

		UVerticalBox* VB_Window = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_SettingsRoot")));
		Window->SetContent(VB_Window);

		// Tabs placeholder (Toggle)
		UBorder* Panel_Tabs = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_Tabs")));
		VB_Window->AddChildToVerticalBox(Panel_Tabs);

		UVerticalBox* VB_Tabs = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Tabs")));
		Panel_Tabs->SetContent(VB_Tabs);

		const FGameplayTag ActionToggle = SafeTag(TEXT("UI.Action.Toggle"));

		EnsureActionButton(BP, VB_Tabs, ActionButtonClass, FName(TEXT("Tab_Audio")),
			FText::FromString(TEXT("Audio")),
			FGameplayTag(),
			ActionToggle,
			FGameplayTag());

		EnsureActionButton(BP, VB_Tabs, ActionButtonClass, FName(TEXT("Tab_Video")),
			FText::FromString(TEXT("Video")),
			FGameplayTag(),
			ActionToggle,
			FGameplayTag());

		EnsureActionButton(BP, VB_Tabs, ActionButtonClass, FName(TEXT("Tab_Controls")),
			FText::FromString(TEXT("Controls")),
			FGameplayTag(),
			ActionToggle,
			FGameplayTag());

		EnsureActionButton(BP, VB_Tabs, ActionButtonClass, FName(TEXT("Tab_Gameplay")),
			FText::FromString(TEXT("Gameplay")),
			FGameplayTag(),
			ActionToggle,
			FGameplayTag());

		// Body placeholder
		UBorder* Panel_Body = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_SettingsBody")));
		VB_Window->AddChildToVerticalBox(Panel_Body);

		// Buttons
		UVerticalBox* VB_Buttons = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Buttons")));
		VB_Window->AddChildToVerticalBox(VB_Buttons);

		const FGameplayTag FocusPrimary = SafeTag(TEXT("Focus.Primary"));
		const FGameplayTag ActionApply = SafeTag(TEXT("UI.Action.Apply"));
		const FGameplayTag ActionCancel = SafeTag(TEXT("UI.Action.Cancel"));

		EnsureActionButton(BP, VB_Buttons, ActionButtonClass, FName(TEXT("Button_Apply")),
			FText::FromString(TEXT("Apply")),
			FocusPrimary,
			ActionApply,
			FGameplayTag());

		EnsureActionButton(BP, VB_Buttons, ActionButtonClass, FName(TEXT("Button_Cancel")),
			FText::FromString(TEXT("Cancel")),
			FGameplayTag(),
			ActionCancel,
			FGameplayTag());

		return true;
	}

	// ------------------------------------------------------------
	// Overlays (minimal scaffolds)
	// ------------------------------------------------------------

	static bool ApplySimpleOverlay(UWidgetBlueprint* BP, const FString& ExpectedName, const FName PanelName)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != ExpectedName)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] %s root exists but isn't a CanvasPanel. Skipping."), *ExpectedName);
			return false;
		}

		UBorder* Panel = InjectFullBorder(BP, Root, PanelName);
		if (!Panel)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] %s already has layout. Skipping injection."), *ExpectedName);
			return false;
		}

		return true;
	}

	static bool ApplyRecipe_Ov_LoadingBlocker(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_LoadingBlocker"), FName(TEXT("Panel_LoadingBlocker"))); }
	static bool ApplyRecipe_Ov_ControllerDisconnected(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_ControllerDisconnected"), FName(TEXT("Panel_ControllerDisconnected"))); }
	static bool ApplyRecipe_Ov_NetworkStatus(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_NetworkStatus"), FName(TEXT("Panel_NetworkStatus"))); }
	static bool ApplyRecipe_Ov_Toast(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_Toast"), FName(TEXT("Panel_ToastRoot"))); }
	static bool ApplyRecipe_Ov_RunLoading(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_RunLoading"), FName(TEXT("Panel_RunLoading"))); }
	static bool ApplyRecipe_Ov_ReadyCheck(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_ReadyCheck"), FName(TEXT("Panel_ReadyCheck"))); }
	static bool ApplyRecipe_Ov_Altar_IdolSelect(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_Altar_IdolSelect"), FName(TEXT("Panel_Altar_IdolSelect"))); }
	static bool ApplyRecipe_Ov_SpeedrunTimer(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_SpeedrunTimer"), FName(TEXT("Panel_SpeedrunTimer"))); }
	static bool ApplyRecipe_Ov_SlotPopup(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_SlotPopup"), FName(TEXT("Panel_SlotPopup"))); }
	static bool ApplyRecipe_Ov_LootPickup(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_LootPickup"), FName(TEXT("Panel_LootPickup"))); }
	static bool ApplyRecipe_Ov_Map(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_Map"), FName(TEXT("Panel_Map"))); }
	static bool ApplyRecipe_Ov_MediaViewer(UWidgetBlueprint* BP) { return ApplySimpleOverlay(BP, TEXT("WBP_Ov_MediaViewer"), FName(TEXT("Panel_MediaViewer"))); }

	// ------------------------------------------------------------
	// Tooltips (minimal scaffolds)
	// ------------------------------------------------------------

	static bool ApplySimpleTooltip(UWidgetBlueprint* BP, const FString& ExpectedName, const FName PanelName)
	{
		if (!BP || !BP->WidgetTree || BP->GetName() != ExpectedName)
		{
			return false;
		}

		BP->Modify();

		UCanvasPanel* Root = EnsureRootCanvas_IfEmpty(BP, FName(TEXT("Root_Canvas")));
		if (!Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] %s root exists but isn't a CanvasPanel. Skipping."), *ExpectedName);
			return false;
		}

		if (!RootCanvasIsInjectable(Root))
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] %s already has layout. Skipping injection."), *ExpectedName);
			return false;
		}

		UBorder* Panel = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), PanelName);
		Root->AddChild(Panel);

		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Panel->Slot))
		{
			// Top-left, autosize (tooltip content will size it)
			Slot->SetAnchors(FAnchors(0.f, 0.f));
			Slot->SetAlignment(FVector2D(0.f, 0.f));
			Slot->SetPosition(FVector2D(0.f, 0.f));
			Slot->SetAutoSize(true);
		}

		// Inner container
		UVerticalBox* VB = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_TooltipLines")));
		Panel->SetContent(VB);

		return true;
	}

	static bool ApplyRecipe_Tooltip_Generic(UWidgetBlueprint* BP) { return ApplySimpleTooltip(BP, TEXT("WBP_Tooltip_Generic"), FName(TEXT("Panel_Tooltip_Generic"))); }
	static bool ApplyRecipe_Tooltip_Item(UWidgetBlueprint* BP) { return ApplySimpleTooltip(BP, TEXT("WBP_Tooltip_Item"), FName(TEXT("Panel_Tooltip_Item"))); }
	static bool ApplyRecipe_Tooltip_Idol(UWidgetBlueprint* BP) { return ApplySimpleTooltip(BP, TEXT("WBP_Tooltip_Idol"), FName(TEXT("Panel_Tooltip_Idol"))); }
	static bool ApplyRecipe_Tooltip_Enemy(UWidgetBlueprint* BP) { return ApplySimpleTooltip(BP, TEXT("WBP_Tooltip_Enemy"), FName(TEXT("Panel_Tooltip_Enemy"))); }
}

namespace T66WidgetLayoutRecipes
{
	bool TryApplyRecipe(UWidgetBlueprint* WidgetBP)
	{
		if (!WidgetBP)
		{
			return false;
		}

		const FString Name = WidgetBP->GetName();

		if (Name == TEXT("WBP_Screen_BootIntro"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_BootIntro(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_MainMenu"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_MainMenu(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_PartySizePicker"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_PartySizePicker(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_HeroSelect_Solo"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_HeroSelect_Solo(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_HeroSelect_Coop"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_HeroSelect_Coop(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_CompanionSelect"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_CompanionSelect(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_SaveSlots"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_SaveSlots(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_SavePreview"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_SavePreview(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_Lobby_New"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Lobby_New(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_Lobby_Load"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Lobby_Load(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_InRunHUD"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_InRunHUD(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_PauseMenu"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_PauseMenu(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_Achievements"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Achievements(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_AccountStatus"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_AccountStatus(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_PlayerSummaryPicker"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_PlayerSummaryPicker(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_RunDetails"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_RunDetails(WidgetBP);
		}

		if (Name == TEXT("WBP_Screen_Results_ChapterEnd"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Results_ChapterEnd(WidgetBP);
		}



		// ------------------------------------------------------------
		// ✅ Modals
		// ------------------------------------------------------------
		if (Name == TEXT("WBP_Modal_GenericBlocker"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Modal_GenericBlocker(WidgetBP);
		}

		if (Name == TEXT("WBP_Modal_DeleteSaveConfirm"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Modal_DeleteSaveConfirm(WidgetBP);
		}

		if (Name == TEXT("WBP_Modal_ReportRun"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Modal_ReportRun(WidgetBP);
		}

		if (Name == TEXT("WBP_Modal_ReportBug"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Modal_ReportBug(WidgetBP);
		}

		if (Name == TEXT("WBP_Modal_QuitConfirm"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Modal_QuitConfirm(WidgetBP);
		}

		if (Name == TEXT("WBP_Modal_Language"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Modal_Language(WidgetBP);
		}

		if (Name == TEXT("WBP_Modal_Settings"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Modal_Settings(WidgetBP);
		}

		// ------------------------------------------------------------
		// ✅ Overlays
		// ------------------------------------------------------------
		if (Name == TEXT("WBP_Ov_LoadingBlocker"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_LoadingBlocker(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_ControllerDisconnected"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_ControllerDisconnected(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_NetworkStatus"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_NetworkStatus(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_Toast"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_Toast(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_RunLoading"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_RunLoading(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_ReadyCheck"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_ReadyCheck(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_Altar_IdolSelect"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_Altar_IdolSelect(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_SpeedrunTimer"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_SpeedrunTimer(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_SlotPopup"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_SlotPopup(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_LootPickup"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_LootPickup(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_Map"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_Map(WidgetBP);
		}

		if (Name == TEXT("WBP_Ov_MediaViewer"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Ov_MediaViewer(WidgetBP);
		}

		// ------------------------------------------------------------
		// ✅ Tooltips
		// ------------------------------------------------------------
		if (Name == TEXT("WBP_Tooltip_Generic"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Tooltip_Generic(WidgetBP);
		}

		if (Name == TEXT("WBP_Tooltip_Item"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Tooltip_Item(WidgetBP);
		}

		if (Name == TEXT("WBP_Tooltip_Idol"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Tooltip_Idol(WidgetBP);
		}

		if (Name == TEXT("WBP_Tooltip_Enemy"))
		{
			return T66WidgetLayoutRecipes_Internal::ApplyRecipe_Tooltip_Enemy(WidgetBP);
		}
		UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutRecipes] No recipe for: %s"), *Name);
		return false;
	}
}
