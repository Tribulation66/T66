#include "T66MainMenuRebuildCommandlet.h"

#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "EditorAssetLibrary.h"
#include "GameplayTagContainer.h"
#include "UObject/UnrealType.h"
#include "WidgetBlueprint.h"

static const TCHAR* T66_MAINMENU_WBP_PATH = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_MainMenu.WBP_Screen_MainMenu");

namespace
{
	static bool SetGameplayTagProperty(UObject* Obj, const FName PropertyName, const FGameplayTag& Tag)
	{
		if (!Obj)
		{
			return false;
		}

		FProperty* Prop = Obj->GetClass()->FindPropertyByName(PropertyName);
		FStructProperty* StructProp = CastField<FStructProperty>(Prop);
		if (!StructProp || StructProp->Struct != TBaseStructure<FGameplayTag>::Get())
		{
			return false;
		}

		FGameplayTag* TagPtr = StructProp->ContainerPtrToValuePtr<FGameplayTag>(Obj);
		if (!TagPtr)
		{
			return false;
		}

		*TagPtr = Tag;
		return true;
	}

	static bool SetTextProperty(UObject* Obj, const FName PropertyName, const FText& Text)
	{
		if (!Obj)
		{
			return false;
		}

		FProperty* Prop = Obj->GetClass()->FindPropertyByName(PropertyName);
		FTextProperty* TextProp = CastField<FTextProperty>(Prop);
		if (!TextProp)
		{
			return false;
		}

		FText* TextPtr = TextProp->ContainerPtrToValuePtr<FText>(Obj);
		if (!TextPtr)
		{
			return false;
		}

		*TextPtr = Text;
		return true;
	}

	static void EnsureWidgetGuid(UWidgetBlueprint* BP, UWidget* Widget)
	{
		if (BP && Widget && !BP->WidgetVariableNameToGuidMap.Contains(Widget->GetFName()))
		{
			BP->OnVariableAdded(Widget->GetFName());
		}
	}
}

UT66MainMenuRebuildCommandlet::UT66MainMenuRebuildCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66MainMenuRebuildCommandlet::Main(const FString& Params)
{
	UObject* Loaded = UEditorAssetLibrary::LoadAsset(T66_MAINMENU_WBP_PATH);
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
	if (!BP)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66MainMenuRebuild] Failed to load WBP_Screen_MainMenu at: %s"), T66_MAINMENU_WBP_PATH);
		return 1;
	}

	BP->Modify();

	UWidgetTree* NewTree = NewObject<UWidgetTree>(BP, UWidgetTree::StaticClass(), NAME_None, RF_Transactional);
	if (!NewTree)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66MainMenuRebuild] Failed to create widget tree"));
		return 1;
	}

	BP->WidgetTree = NewTree;

	// Root canvas fills entire screen
	UCanvasPanel* Root = BP->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName(TEXT("Root_Canvas")));
	BP->WidgetTree->RootWidget = Root;

	// Main horizontal layout - left panel for menu, right panel for leaderboard
	UHorizontalBox* HB = BP->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_MainMenu")));
	Root->AddChild(HB);
	if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(HB->Slot))
	{
		Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		Slot->SetOffsets(FMargin(0.f));
		Slot->SetAlignment(FVector2D(0.f, 0.f));
	}

	// Left panel (menu) - semi-transparent dark background
	UBorder* PanelLeft = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_LeftMenu")));
	PanelLeft->SetBrushColor(FLinearColor(0.02f, 0.02f, 0.05f, 0.85f));
	PanelLeft->SetPadding(FMargin(40.f, 30.f, 40.f, 30.f));
	HB->AddChildToHorizontalBox(PanelLeft);

	// Right panel (leaderboard placeholder) - slightly lighter
	UBorder* PanelRight = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_RightLeaderboard")));
	PanelRight->SetBrushColor(FLinearColor(0.03f, 0.03f, 0.06f, 0.7f));
	HB->AddChildToHorizontalBox(PanelRight);

	// Set both panels to fill equally
	if (UHorizontalBoxSlot* LeftSlot = Cast<UHorizontalBoxSlot>(PanelLeft->Slot))
	{
		FSlateChildSize FillSize;
		FillSize.SizeRule = ESlateSizeRule::Fill;
		FillSize.Value = 1.f;
		LeftSlot->SetSize(FillSize);
		LeftSlot->SetHorizontalAlignment(HAlign_Fill);
		LeftSlot->SetVerticalAlignment(VAlign_Fill);
	}
	if (UHorizontalBoxSlot* RightSlot = Cast<UHorizontalBoxSlot>(PanelRight->Slot))
	{
		FSlateChildSize FillSize;
		FillSize.SizeRule = ESlateSizeRule::Fill;
		FillSize.Value = 1.f;
		RightSlot->SetSize(FillSize);
		RightSlot->SetHorizontalAlignment(HAlign_Fill);
		RightSlot->SetVerticalAlignment(VAlign_Fill);
	}

	// Left panel vertical layout
	UVerticalBox* VB_Left = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_LeftMenu")));
	PanelLeft->SetContent(VB_Left);

	// ===== TITLE TEXT =====
	UTextBlock* TitleText = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Title")));
	TitleText->SetText(FText::FromString(TEXT("Main Menu")));
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	
	FSlateFontInfo TitleFont = TitleText->GetFont();
	TitleFont.Size = 36;
	TitleText->SetFont(TitleFont);
	
	VB_Left->AddChildToVerticalBox(TitleText);
	if (UVerticalBoxSlot* TitleSlot = Cast<UVerticalBoxSlot>(TitleText->Slot))
	{
		TitleSlot->SetHorizontalAlignment(HAlign_Left);
		TitleSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 30.f));
	}

	// Buttons container
	UVerticalBox* VB_Buttons = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_MainMenuButtons")));
	VB_Left->AddChildToVerticalBox(VB_Buttons);
	if (UVerticalBoxSlot* ButtonsSlot = Cast<UVerticalBoxSlot>(VB_Buttons->Slot))
	{
		ButtonsSlot->SetHorizontalAlignment(HAlign_Left);
	}

	// Icon buttons container (bottom)
	UHorizontalBox* HB_Icons = BP->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_MainMenuIcons")));
	VB_Left->AddChildToVerticalBox(HB_Icons);
	if (UVerticalBoxSlot* IconsSlot = Cast<UVerticalBoxSlot>(HB_Icons->Slot))
	{
		IconsSlot->SetHorizontalAlignment(HAlign_Left);
		IconsSlot->SetPadding(FMargin(0.f, 30.f, 0.f, 0.f));
	}

	int32 ButtonCount = 0;

	// Helper lambda to create a button with embedded text directly
	auto CreateInlineButton = [&](const FName Name, const FText& Label, float Width, float Height) -> UWidget*
	{
		// Create SizeBox as container
		USizeBox* SizeBox = BP->WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(*(Name.ToString() + TEXT("_SizeBox"))));
		SizeBox->SetMinDesiredWidth(Width);
		SizeBox->SetMinDesiredHeight(Height);
		
		// Create Button
		UButton* Button = BP->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(*(Name.ToString() + TEXT("_Btn"))));
		SizeBox->SetContent(Button);
		
		// Style the button
		FButtonStyle Style = Button->GetStyle();
		Style.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.2f, 1.f));
		Style.Normal.DrawAs = ESlateBrushDrawType::Box;
		Style.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.25f, 0.4f, 1.f));
		Style.Hovered.DrawAs = ESlateBrushDrawType::Box;
		Style.Pressed.TintColor = FSlateColor(FLinearColor(0.3f, 0.35f, 0.5f, 1.f));
		Style.Pressed.DrawAs = ESlateBrushDrawType::Box;
		Button->SetStyle(Style);
		
		// Create overlay for content
		UOverlay* Overlay = BP->WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName(*(Name.ToString() + TEXT("_Overlay"))));
		Button->SetContent(Overlay);
		
		// Add text
		UTextBlock* Text = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(*(Name.ToString() + TEXT("_Text"))));
		Text->SetText(Label);
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = 18;
		Text->SetFont(Font);
		Text->SetJustification(ETextJustify::Center);
		Overlay->AddChild(Text);
		
		if (UOverlaySlot* TextSlot = Cast<UOverlaySlot>(Text->Slot))
		{
			TextSlot->SetHorizontalAlignment(HAlign_Center);
			TextSlot->SetVerticalAlignment(VAlign_Center);
		}
		
		// Register all widgets
		EnsureWidgetGuid(BP, SizeBox);
		EnsureWidgetGuid(BP, Button);
		EnsureWidgetGuid(BP, Overlay);
		EnsureWidgetGuid(BP, Text);
		
		return SizeBox;
	};

	auto AddActionButton = [&](const FName Name, const FText& Label)
	{
		UWidget* Button = CreateInlineButton(Name, Label, 250.f, 50.f);
		if (!Button)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66MainMenuRebuild] Failed to construct button: %s"), *Name.ToString());
			return;
		}

		VB_Buttons->AddChildToVerticalBox(Button);
		Button->SetVisibility(ESlateVisibility::Visible);

		if (UVerticalBoxSlot* SlotVB = Cast<UVerticalBoxSlot>(Button->Slot))
		{
			SlotVB->SetHorizontalAlignment(HAlign_Left);
			SlotVB->SetPadding(FMargin(0.f, 6.f));
		}

		ButtonCount++;
		UE_LOG(LogTemp, Display, TEXT("[T66MainMenuRebuild] Added button: %s (%s)"), *Name.ToString(), *Label.ToString());
	};

	auto AddIconButton = [&](const FName Name, const FText& IconText)
	{
		UWidget* Button = CreateInlineButton(Name, IconText, 50.f, 50.f);
		if (!Button)
		{
			return;
		}

		HB_Icons->AddChildToHorizontalBox(Button);
		Button->SetVisibility(ESlateVisibility::Visible);

		if (UHorizontalBoxSlot* SlotHB = Cast<UHorizontalBoxSlot>(Button->Slot))
		{
			SlotHB->SetPadding(FMargin(0.f, 0.f, 12.f, 0.f));
		}

		ButtonCount++;
		UE_LOG(LogTemp, Display, TEXT("[T66MainMenuRebuild] Added icon button: %s"), *Name.ToString());
	};

	// Add main menu action buttons
	AddActionButton(FName(TEXT("Button_NewGame")), FText::FromString(TEXT("New Game")));
	AddActionButton(FName(TEXT("Button_LoadGame")), FText::FromString(TEXT("Load Game")));
	AddActionButton(FName(TEXT("Button_Settings")), FText::FromString(TEXT("Settings")));
	AddActionButton(FName(TEXT("Button_Achievements")), FText::FromString(TEXT("Achievements")));
	AddActionButton(FName(TEXT("Button_AccountStatus")), FText::FromString(TEXT("Account Status")));
	
	// Add icon buttons at bottom
	AddIconButton(FName(TEXT("Button_LanguageIcon")), FText::FromString(TEXT("🌐")));
	AddIconButton(FName(TEXT("Button_QuitIcon")), FText::FromString(TEXT("✕")));

	// Register all widgets
	EnsureWidgetGuid(BP, Root);
	EnsureWidgetGuid(BP, HB);
	EnsureWidgetGuid(BP, PanelLeft);
	EnsureWidgetGuid(BP, PanelRight);
	EnsureWidgetGuid(BP, VB_Left);
	EnsureWidgetGuid(BP, TitleText);
	EnsureWidgetGuid(BP, VB_Buttons);
	EnsureWidgetGuid(BP, HB_Icons);

	BP->MarkPackageDirty();
	const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(BP, /*bOnlyIfIsDirty=*/true);

	UE_LOG(LogTemp, Display, TEXT("[T66MainMenuRebuild] Saved=%s | Buttons=%d"), bSaved ? TEXT("true") : TEXT("false"), ButtonCount);
	return bSaved ? 0 : 1;
}
