#include "T66ScreenSetupCommandlet.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
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
#include "WidgetBlueprint.h"

// Screen paths
static const TCHAR* PATH_PARTYSIZE = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_PartySizePicker.WBP_Screen_PartySizePicker");
static const TCHAR* PATH_HEROSELECT_SOLO = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_HeroSelect_Solo.WBP_Screen_HeroSelect_Solo");
static const TCHAR* PATH_HEROSELECT_COOP = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_HeroSelect_Coop.WBP_Screen_HeroSelect_Coop");
static const TCHAR* PATH_SAVESLOTS = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_SaveSlots.WBP_Screen_SaveSlots");
static const TCHAR* PATH_ACHIEVEMENTS = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_Achievements.WBP_Screen_Achievements");
static const TCHAR* PATH_ACCOUNTSTATUS = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_AccountStatus.WBP_Screen_AccountStatus");

namespace
{
	static void EnsureWidgetGuid(UWidgetBlueprint* BP, UWidget* Widget)
	{
		if (BP && Widget && !BP->WidgetVariableNameToGuidMap.Contains(Widget->GetFName()))
		{
			BP->OnVariableAdded(Widget->GetFName());
		}
	}

	// Creates a styled button with text
	static UWidget* CreateButton(UWidgetBlueprint* BP, const FName& Name, const FText& Label, float Width, float Height)
	{
		USizeBox* SizeBox = BP->WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(*(Name.ToString() + TEXT("_SizeBox"))));
		SizeBox->SetMinDesiredWidth(Width);
		SizeBox->SetMinDesiredHeight(Height);
		
		UButton* Button = BP->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(*(Name.ToString() + TEXT("_Btn"))));
		SizeBox->SetContent(Button);
		
		FButtonStyle Style = Button->GetStyle();
		Style.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.2f, 1.f));
		Style.Normal.DrawAs = ESlateBrushDrawType::Box;
		Style.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.25f, 0.4f, 1.f));
		Style.Hovered.DrawAs = ESlateBrushDrawType::Box;
		Style.Pressed.TintColor = FSlateColor(FLinearColor(0.3f, 0.35f, 0.5f, 1.f));
		Style.Pressed.DrawAs = ESlateBrushDrawType::Box;
		Button->SetStyle(Style);
		
		UOverlay* Overlay = BP->WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName(*(Name.ToString() + TEXT("_Overlay"))));
		Button->SetContent(Overlay);
		
		UTextBlock* Text = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(*(Name.ToString() + TEXT("_Text"))));
		Text->SetText(Label);
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = 20;
		Text->SetFont(Font);
		Text->SetJustification(ETextJustify::Center);
		Overlay->AddChild(Text);
		
		if (UOverlaySlot* TextSlot = Cast<UOverlaySlot>(Text->Slot))
		{
			TextSlot->SetHorizontalAlignment(HAlign_Center);
			TextSlot->SetVerticalAlignment(VAlign_Center);
		}
		
		EnsureWidgetGuid(BP, SizeBox);
		EnsureWidgetGuid(BP, Button);
		EnsureWidgetGuid(BP, Overlay);
		EnsureWidgetGuid(BP, Text);
		
		return SizeBox;
	}

	// Creates screen base structure: Canvas > Border (dark bg) > VBox with title
	static UVerticalBox* CreateScreenBase(UWidgetBlueprint* BP, const FString& TitleText)
	{
		BP->Modify();
		BP->WidgetVariableNameToGuidMap.Empty();

		UWidgetTree* NewTree = NewObject<UWidgetTree>(BP, UWidgetTree::StaticClass(), NAME_None, RF_Transactional);
		BP->WidgetTree = NewTree;

		UCanvasPanel* Root = BP->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName(TEXT("Root_Canvas")));
		BP->WidgetTree->RootWidget = Root;

		UBorder* Background = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Background")));
		Background->SetBrushColor(FLinearColor(0.02f, 0.02f, 0.05f, 0.95f));
		Background->SetPadding(FMargin(60.f, 40.f));
		Root->AddChild(Background);

		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Background->Slot))
		{
			Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			Slot->SetOffsets(FMargin(0.f));
		}

		UVerticalBox* VB_Main = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Main")));
		Background->SetContent(VB_Main);

		// Title
		UTextBlock* Title = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_ScreenTitle")));
		Title->SetText(FText::FromString(TitleText));
		Title->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo TitleFont = Title->GetFont();
		TitleFont.Size = 36;
		Title->SetFont(TitleFont);
		VB_Main->AddChildToVerticalBox(Title);

		if (UVerticalBoxSlot* TitleSlot = Cast<UVerticalBoxSlot>(Title->Slot))
		{
			TitleSlot->SetHorizontalAlignment(HAlign_Center);
			TitleSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 40.f));
		}

		EnsureWidgetGuid(BP, Root);
		EnsureWidgetGuid(BP, Background);
		EnsureWidgetGuid(BP, VB_Main);
		EnsureWidgetGuid(BP, Title);

		return VB_Main;
	}

	static bool SaveScreen(UWidgetBlueprint* BP, const TCHAR* Path)
	{
		BP->MarkPackageDirty();
		const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(BP, true);
		UE_LOG(LogTemp, Display, TEXT("[T66ScreenSetup] %s: Saved=%s"), Path, bSaved ? TEXT("true") : TEXT("false"));
		return bSaved;
	}
}

UT66ScreenSetupCommandlet::UT66ScreenSetupCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66ScreenSetupCommandlet::Main(const FString& Params)
{
	SetupPartySizePicker();
	SetupHeroSelectSolo();
	SetupHeroSelectCoop();
	SetupSaveSlots();
	SetupAchievements();
	SetupAccountStatus();

	UE_LOG(LogTemp, Display, TEXT("[T66ScreenSetup] All screens setup complete"));
	return 0;
}

void UT66ScreenSetupCommandlet::SetupPartySizePicker()
{
	UObject* Loaded = UEditorAssetLibrary::LoadAsset(PATH_PARTYSIZE);
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
	if (!BP)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66ScreenSetup] Failed to load: %s"), PATH_PARTYSIZE);
		return;
	}

	UVerticalBox* VB_Main = CreateScreenBase(BP, TEXT("Select Party Size"));

	// Subtitle
	UTextBlock* Subtitle = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Subtitle")));
	Subtitle->SetText(FText::FromString(TEXT("Choose how you want to play")));
	Subtitle->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.f)));
	FSlateFontInfo SubFont = Subtitle->GetFont();
	SubFont.Size = 18;
	Subtitle->SetFont(SubFont);
	VB_Main->AddChildToVerticalBox(Subtitle);

	if (UVerticalBoxSlot* SubSlot = Cast<UVerticalBoxSlot>(Subtitle->Slot))
	{
		SubSlot->SetHorizontalAlignment(HAlign_Center);
		SubSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 60.f));
	}
	EnsureWidgetGuid(BP, Subtitle);

	// Buttons container (horizontal)
	UHorizontalBox* HB_Buttons = BP->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_Buttons")));
	VB_Main->AddChildToVerticalBox(HB_Buttons);

	if (UVerticalBoxSlot* BtnSlot = Cast<UVerticalBoxSlot>(HB_Buttons->Slot))
	{
		BtnSlot->SetHorizontalAlignment(HAlign_Center);
	}
	EnsureWidgetGuid(BP, HB_Buttons);

	// Solo button (1 player icon)
	UWidget* SoloBtn = CreateButton(BP, FName(TEXT("Button_Solo")), FText::FromString(TEXT("Solo\n👤")), 180.f, 100.f);
	HB_Buttons->AddChildToHorizontalBox(SoloBtn);
	if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(SoloBtn->Slot))
	{
		Slot->SetPadding(FMargin(0.f, 0.f, 30.f, 0.f));
	}

	// Duo button (2 player icons)
	UWidget* DuoBtn = CreateButton(BP, FName(TEXT("Button_Duo")), FText::FromString(TEXT("Duo\n👤👤")), 180.f, 100.f);
	HB_Buttons->AddChildToHorizontalBox(DuoBtn);
	if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(DuoBtn->Slot))
	{
		Slot->SetPadding(FMargin(0.f, 0.f, 30.f, 0.f));
	}

	// Trio button (3 player icons)
	UWidget* TrioBtn = CreateButton(BP, FName(TEXT("Button_Trio")), FText::FromString(TEXT("Trio\n👤👤👤")), 180.f, 100.f);
	HB_Buttons->AddChildToHorizontalBox(TrioBtn);

	// Back button at bottom left
	UWidget* BackBtn = CreateButton(BP, FName(TEXT("Button_Back")), FText::FromString(TEXT("Back")), 120.f, 45.f);
	VB_Main->AddChildToVerticalBox(BackBtn);
	if (UVerticalBoxSlot* BackSlot = Cast<UVerticalBoxSlot>(BackBtn->Slot))
	{
		BackSlot->SetHorizontalAlignment(HAlign_Left);
		BackSlot->SetPadding(FMargin(0.f, 60.f, 0.f, 0.f));
	}

	SaveScreen(BP, PATH_PARTYSIZE);
}

void UT66ScreenSetupCommandlet::SetupHeroSelectSolo()
{
	UObject* Loaded = UEditorAssetLibrary::LoadAsset(PATH_HEROSELECT_SOLO);
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
	if (!BP)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66ScreenSetup] Failed to load: %s"), PATH_HEROSELECT_SOLO);
		return;
	}

	UVerticalBox* VB_Main = CreateScreenBase(BP, TEXT("Hero Selection"));

	// Top bar with Hero Belt placeholder and Hero Grid button
	UHorizontalBox* HB_TopBar = BP->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_TopBar")));
	VB_Main->AddChildToVerticalBox(HB_TopBar);
	if (UVerticalBoxSlot* TopSlot = Cast<UVerticalBoxSlot>(HB_TopBar->Slot))
	{
		TopSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 20.f));
	}
	EnsureWidgetGuid(BP, HB_TopBar);

	// Hero Grid button
	UWidget* GridBtn = CreateButton(BP, FName(TEXT("Button_HeroGrid")), FText::FromString(TEXT("HERO GRID")), 140.f, 40.f);
	HB_TopBar->AddChildToHorizontalBox(GridBtn);

	// Hero Belt placeholder
	UTextBlock* BeltPlaceholder = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_HeroBelt")));
	BeltPlaceholder->SetText(FText::FromString(TEXT("◀  ⬤ ⬤ ⬤ [HERO] ⬤ ⬤ ⬤  ▶")));
	BeltPlaceholder->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.6f, 0.7f, 1.f)));
	FSlateFontInfo BeltFont = BeltPlaceholder->GetFont();
	BeltFont.Size = 24;
	BeltPlaceholder->SetFont(BeltFont);
	HB_TopBar->AddChildToHorizontalBox(BeltPlaceholder);
	if (UHorizontalBoxSlot* BeltSlot = Cast<UHorizontalBoxSlot>(BeltPlaceholder->Slot))
	{
		BeltSlot->SetPadding(FMargin(40.f, 0.f, 0.f, 0.f));
		BeltSlot->SetVerticalAlignment(VAlign_Center);
	}
	EnsureWidgetGuid(BP, BeltPlaceholder);

	// Main content area: Left (Skins) | Center (Preview) | Right (Info)
	UHorizontalBox* HB_Content = BP->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_Content")));
	VB_Main->AddChildToVerticalBox(HB_Content);
	if (UVerticalBoxSlot* ContentSlot = Cast<UVerticalBoxSlot>(HB_Content->Slot))
	{
		FSlateChildSize FillSize;
		FillSize.SizeRule = ESlateSizeRule::Fill;
		FillSize.Value = 1.f;
		ContentSlot->SetSize(FillSize);
	}
	EnsureWidgetGuid(BP, HB_Content);

	// Left Panel - Skins
	UBorder* LeftPanel = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_Skins")));
	LeftPanel->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.08f, 0.8f));
	LeftPanel->SetPadding(FMargin(15.f));
	HB_Content->AddChildToHorizontalBox(LeftPanel);
	if (UHorizontalBoxSlot* LeftSlot = Cast<UHorizontalBoxSlot>(LeftPanel->Slot))
	{
		FSlateChildSize AutoSize;
		AutoSize.SizeRule = ESlateSizeRule::Automatic;
		LeftSlot->SetSize(AutoSize);
	}
	EnsureWidgetGuid(BP, LeftPanel);

	UVerticalBox* VB_Skins = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Skins")));
	LeftPanel->SetContent(VB_Skins);
	EnsureWidgetGuid(BP, VB_Skins);

	UTextBlock* SkinsTitle = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_SkinsTitle")));
	SkinsTitle->SetText(FText::FromString(TEXT("SKINS")));
	SkinsTitle->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo SkinsFont = SkinsTitle->GetFont();
	SkinsFont.Size = 18;
	SkinsTitle->SetFont(SkinsFont);
	VB_Skins->AddChildToVerticalBox(SkinsTitle);
	EnsureWidgetGuid(BP, SkinsTitle);

	UTextBlock* SkinsList = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_SkinsList")));
	SkinsList->SetText(FText::FromString(TEXT("Default Skin ✓\nFire Skin [BUY]\nIce Skin [BUY]\nGold Skin [BUY]")));
	SkinsList->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.f)));
	VB_Skins->AddChildToVerticalBox(SkinsList);
	if (UVerticalBoxSlot* ListSlot = Cast<UVerticalBoxSlot>(SkinsList->Slot))
	{
		ListSlot->SetPadding(FMargin(0.f, 15.f, 0.f, 0.f));
	}
	EnsureWidgetGuid(BP, SkinsList);

	// Center Panel - 3D Preview
	UBorder* CenterPanel = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_Preview")));
	CenterPanel->SetBrushColor(FLinearColor(0.03f, 0.03f, 0.05f, 0.5f));
	HB_Content->AddChildToHorizontalBox(CenterPanel);
	if (UHorizontalBoxSlot* CenterSlot = Cast<UHorizontalBoxSlot>(CenterPanel->Slot))
	{
		FSlateChildSize FillSize;
		FillSize.SizeRule = ESlateSizeRule::Fill;
		FillSize.Value = 1.f;
		CenterSlot->SetSize(FillSize);
		CenterSlot->SetPadding(FMargin(20.f, 0.f));
	}
	EnsureWidgetGuid(BP, CenterPanel);

	UVerticalBox* VB_Preview = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Preview")));
	CenterPanel->SetContent(VB_Preview);
	EnsureWidgetGuid(BP, VB_Preview);

	UTextBlock* PreviewText = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Preview")));
	PreviewText->SetText(FText::FromString(TEXT("[ 3D Hero Preview ]\n\nDrag to rotate")));
	PreviewText->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.6f, 1.f)));
	PreviewText->SetJustification(ETextJustify::Center);
	FSlateFontInfo PreviewFont = PreviewText->GetFont();
	PreviewFont.Size = 20;
	PreviewText->SetFont(PreviewFont);
	VB_Preview->AddChildToVerticalBox(PreviewText);
	if (UVerticalBoxSlot* PrevSlot = Cast<UVerticalBoxSlot>(PreviewText->Slot))
	{
		PrevSlot->SetHorizontalAlignment(HAlign_Center);
		PrevSlot->SetVerticalAlignment(VAlign_Center);
		FSlateChildSize FillSize;
		FillSize.SizeRule = ESlateSizeRule::Fill;
		FillSize.Value = 1.f;
		PrevSlot->SetSize(FillSize);
	}
	EnsureWidgetGuid(BP, PreviewText);

	// Companion button below preview
	UWidget* CompanionBtn = CreateButton(BP, FName(TEXT("Button_Companion")), FText::FromString(TEXT("CHOOSE COMPANION")), 200.f, 45.f);
	VB_Preview->AddChildToVerticalBox(CompanionBtn);
	if (UVerticalBoxSlot* CompSlot = Cast<UVerticalBoxSlot>(CompanionBtn->Slot))
	{
		CompSlot->SetHorizontalAlignment(HAlign_Center);
		CompSlot->SetPadding(FMargin(0.f, 20.f, 0.f, 0.f));
	}

	// Right Panel - Hero Info
	UBorder* RightPanel = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Panel_HeroInfo")));
	RightPanel->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.08f, 0.8f));
	RightPanel->SetPadding(FMargin(20.f));
	HB_Content->AddChildToHorizontalBox(RightPanel);
	if (UHorizontalBoxSlot* RightSlot = Cast<UHorizontalBoxSlot>(RightPanel->Slot))
	{
		FSlateChildSize AutoSize;
		AutoSize.SizeRule = ESlateSizeRule::Automatic;
		RightSlot->SetSize(AutoSize);
	}
	EnsureWidgetGuid(BP, RightPanel);

	UVerticalBox* VB_Info = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_HeroInfo")));
	RightPanel->SetContent(VB_Info);
	EnsureWidgetGuid(BP, VB_Info);

	UTextBlock* HeroName = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_HeroName")));
	HeroName->SetText(FText::FromString(TEXT("THE BERSERKER")));
	HeroName->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo NameFont = HeroName->GetFont();
	NameFont.Size = 28;
	HeroName->SetFont(NameFont);
	VB_Info->AddChildToVerticalBox(HeroName);
	EnsureWidgetGuid(BP, HeroName);

	UTextBlock* HeroDesc = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_HeroDesc")));
	HeroDesc->SetText(FText::FromString(TEXT("A brutal warrior who excels\nin close-quarters combat.\n\n[Video Preview Area]\n\nMedals: ⬤ ⬤ ⬤ ⬤")));
	HeroDesc->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.f)));
	VB_Info->AddChildToVerticalBox(HeroDesc);
	if (UVerticalBoxSlot* DescSlot = Cast<UVerticalBoxSlot>(HeroDesc->Slot))
	{
		DescSlot->SetPadding(FMargin(0.f, 15.f, 0.f, 0.f));
	}
	EnsureWidgetGuid(BP, HeroDesc);

	// Difficulty selector
	UTextBlock* DiffLabel = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Difficulty")));
	DiffLabel->SetText(FText::FromString(TEXT("Difficulty: [Easy] Medium Hard")));
	DiffLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.8f, 0.3f, 1.f)));
	VB_Info->AddChildToVerticalBox(DiffLabel);
	if (UVerticalBoxSlot* DiffSlot = Cast<UVerticalBoxSlot>(DiffLabel->Slot))
	{
		DiffSlot->SetPadding(FMargin(0.f, 30.f, 0.f, 0.f));
	}
	EnsureWidgetGuid(BP, DiffLabel);

	// Bottom buttons bar
	UHorizontalBox* HB_Buttons = BP->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_BottomButtons")));
	VB_Main->AddChildToVerticalBox(HB_Buttons);
	if (UVerticalBoxSlot* BtnBarSlot = Cast<UVerticalBoxSlot>(HB_Buttons->Slot))
	{
		BtnBarSlot->SetPadding(FMargin(0.f, 30.f, 0.f, 0.f));
	}
	EnsureWidgetGuid(BP, HB_Buttons);

	// Back button
	UWidget* BackBtn = CreateButton(BP, FName(TEXT("Button_Back")), FText::FromString(TEXT("Back")), 120.f, 50.f);
	HB_Buttons->AddChildToHorizontalBox(BackBtn);

	// Spacer
	UTextBlock* Spacer = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Spacer")));
	Spacer->SetText(FText::FromString(TEXT("")));
	HB_Buttons->AddChildToHorizontalBox(Spacer);
	if (UHorizontalBoxSlot* SpacerSlot = Cast<UHorizontalBoxSlot>(Spacer->Slot))
	{
		FSlateChildSize FillSize;
		FillSize.SizeRule = ESlateSizeRule::Fill;
		FillSize.Value = 1.f;
		SpacerSlot->SetSize(FillSize);
	}
	EnsureWidgetGuid(BP, Spacer);

	// The Lab button
	UWidget* LabBtn = CreateButton(BP, FName(TEXT("Button_Lab")), FText::FromString(TEXT("THE LAB")), 140.f, 50.f);
	HB_Buttons->AddChildToHorizontalBox(LabBtn);
	if (UHorizontalBoxSlot* LabSlot = Cast<UHorizontalBoxSlot>(LabBtn->Slot))
	{
		LabSlot->SetPadding(FMargin(0.f, 0.f, 20.f, 0.f));
	}

	// Enter Tribulation button
	UWidget* ConfirmBtn = CreateButton(BP, FName(TEXT("Button_Confirm")), FText::FromString(TEXT("ENTER THE TRIBULATION")), 280.f, 60.f);
	HB_Buttons->AddChildToHorizontalBox(ConfirmBtn);

	SaveScreen(BP, PATH_HEROSELECT_SOLO);
}

void UT66ScreenSetupCommandlet::SetupHeroSelectCoop()
{
	UObject* Loaded = UEditorAssetLibrary::LoadAsset(PATH_HEROSELECT_COOP);
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
	if (!BP)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66ScreenSetup] Failed to load: %s"), PATH_HEROSELECT_COOP);
		return;
	}

	UVerticalBox* VB_Main = CreateScreenBase(BP, TEXT("Co-op Hero Selection"));

	// Party roster at top
	UTextBlock* PartyRoster = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_PartyRoster")));
	PartyRoster->SetText(FText::FromString(TEXT("Party: [You - Selecting...] [Player 2 - Waiting...] [Player 3 - Waiting...]")));
	PartyRoster->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.4f, 1.f)));
	FSlateFontInfo RosterFont = PartyRoster->GetFont();
	RosterFont.Size = 16;
	PartyRoster->SetFont(RosterFont);
	VB_Main->AddChildToVerticalBox(PartyRoster);
	if (UVerticalBoxSlot* RosterSlot = Cast<UVerticalBoxSlot>(PartyRoster->Slot))
	{
		RosterSlot->SetHorizontalAlignment(HAlign_Center);
		RosterSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 30.f));
	}
	EnsureWidgetGuid(BP, PartyRoster);

	// Hero belt placeholder
	UTextBlock* BeltPlaceholder = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_HeroBelt")));
	BeltPlaceholder->SetText(FText::FromString(TEXT("◀  ⬤ ⬤ ⬤ [HERO] ⬤ ⬤ ⬤  ▶")));
	BeltPlaceholder->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.6f, 0.7f, 1.f)));
	FSlateFontInfo BeltFont = BeltPlaceholder->GetFont();
	BeltFont.Size = 24;
	BeltPlaceholder->SetFont(BeltFont);
	VB_Main->AddChildToVerticalBox(BeltPlaceholder);
	if (UVerticalBoxSlot* BeltSlot = Cast<UVerticalBoxSlot>(BeltPlaceholder->Slot))
	{
		BeltSlot->SetHorizontalAlignment(HAlign_Center);
		BeltSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 40.f));
	}
	EnsureWidgetGuid(BP, BeltPlaceholder);

	// Hero preview placeholder
	UTextBlock* PreviewText = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Preview")));
	PreviewText->SetText(FText::FromString(TEXT("[ 3D Hero Preview + Companion ]\n\nSkins | Preview | Hero Info")));
	PreviewText->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.6f, 1.f)));
	PreviewText->SetJustification(ETextJustify::Center);
	FSlateFontInfo PreviewFont = PreviewText->GetFont();
	PreviewFont.Size = 18;
	PreviewText->SetFont(PreviewFont);
	VB_Main->AddChildToVerticalBox(PreviewText);
	if (UVerticalBoxSlot* PrevSlot = Cast<UVerticalBoxSlot>(PreviewText->Slot))
	{
		PrevSlot->SetHorizontalAlignment(HAlign_Center);
		FSlateChildSize FillSize;
		FillSize.SizeRule = ESlateSizeRule::Fill;
		FillSize.Value = 1.f;
		PrevSlot->SetSize(FillSize);
	}
	EnsureWidgetGuid(BP, PreviewText);

	// Bottom buttons
	UHorizontalBox* HB_Buttons = BP->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_BottomButtons")));
	VB_Main->AddChildToVerticalBox(HB_Buttons);
	if (UVerticalBoxSlot* BtnSlot = Cast<UVerticalBoxSlot>(HB_Buttons->Slot))
	{
		BtnSlot->SetPadding(FMargin(0.f, 30.f, 0.f, 0.f));
	}
	EnsureWidgetGuid(BP, HB_Buttons);

	// Back button
	UWidget* BackBtn = CreateButton(BP, FName(TEXT("Button_Back")), FText::FromString(TEXT("Back")), 120.f, 50.f);
	HB_Buttons->AddChildToHorizontalBox(BackBtn);

	// Spacer
	UTextBlock* Spacer = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Spacer")));
	Spacer->SetText(FText::FromString(TEXT("")));
	HB_Buttons->AddChildToHorizontalBox(Spacer);
	if (UHorizontalBoxSlot* SpacerSlot = Cast<UHorizontalBoxSlot>(Spacer->Slot))
	{
		FSlateChildSize FillSize;
		FillSize.SizeRule = ESlateSizeRule::Fill;
		FillSize.Value = 1.f;
		SpacerSlot->SetSize(FillSize);
	}
	EnsureWidgetGuid(BP, Spacer);

	// Ready button (for non-host)
	UWidget* ReadyBtn = CreateButton(BP, FName(TEXT("Button_Ready")), FText::FromString(TEXT("READY")), 150.f, 50.f);
	HB_Buttons->AddChildToHorizontalBox(ReadyBtn);
	if (UHorizontalBoxSlot* ReadySlot = Cast<UHorizontalBoxSlot>(ReadyBtn->Slot))
	{
		ReadySlot->SetPadding(FMargin(0.f, 0.f, 20.f, 0.f));
	}

	// Start button (host only)
	UWidget* ConfirmBtn = CreateButton(BP, FName(TEXT("Button_Confirm")), FText::FromString(TEXT("ENTER THE TRIBULATION")), 280.f, 60.f);
	HB_Buttons->AddChildToHorizontalBox(ConfirmBtn);

	SaveScreen(BP, PATH_HEROSELECT_COOP);
}

void UT66ScreenSetupCommandlet::SetupSaveSlots()
{
	UObject* Loaded = UEditorAssetLibrary::LoadAsset(PATH_SAVESLOTS);
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
	if (!BP)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66ScreenSetup] Failed to load: %s"), PATH_SAVESLOTS);
		return;
	}

	UVerticalBox* VB_Main = CreateScreenBase(BP, TEXT("Load Game"));

	// Placeholder
	UTextBlock* Placeholder = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Placeholder")));
	Placeholder->SetText(FText::FromString(TEXT("[ Save Slot List - Coming Soon ]")));
	Placeholder->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.f)));
	FSlateFontInfo PlaceFont = Placeholder->GetFont();
	PlaceFont.Size = 16;
	Placeholder->SetFont(PlaceFont);
	VB_Main->AddChildToVerticalBox(Placeholder);

	if (UVerticalBoxSlot* PlaceSlot = Cast<UVerticalBoxSlot>(Placeholder->Slot))
	{
		PlaceSlot->SetHorizontalAlignment(HAlign_Center);
		PlaceSlot->SetPadding(FMargin(0.f, 80.f));
	}
	EnsureWidgetGuid(BP, Placeholder);

	// Back button
	UWidget* BackBtn = CreateButton(BP, FName(TEXT("Button_Back")), FText::FromString(TEXT("Back")), 150.f, 50.f);
	VB_Main->AddChildToVerticalBox(BackBtn);

	if (UVerticalBoxSlot* BackSlot = Cast<UVerticalBoxSlot>(BackBtn->Slot))
	{
		BackSlot->SetHorizontalAlignment(HAlign_Left);
	}

	SaveScreen(BP, PATH_SAVESLOTS);
}

void UT66ScreenSetupCommandlet::SetupAchievements()
{
	UObject* Loaded = UEditorAssetLibrary::LoadAsset(PATH_ACHIEVEMENTS);
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
	if (!BP)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66ScreenSetup] Failed to load: %s"), PATH_ACHIEVEMENTS);
		return;
	}

	UVerticalBox* VB_Main = CreateScreenBase(BP, TEXT("Achievements"));

	// Placeholder
	UTextBlock* Placeholder = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Placeholder")));
	Placeholder->SetText(FText::FromString(TEXT("[ Achievement List - Coming Soon ]")));
	Placeholder->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.f)));
	FSlateFontInfo PlaceFont = Placeholder->GetFont();
	PlaceFont.Size = 16;
	Placeholder->SetFont(PlaceFont);
	VB_Main->AddChildToVerticalBox(Placeholder);

	if (UVerticalBoxSlot* PlaceSlot = Cast<UVerticalBoxSlot>(Placeholder->Slot))
	{
		PlaceSlot->SetHorizontalAlignment(HAlign_Center);
		PlaceSlot->SetPadding(FMargin(0.f, 80.f));
	}
	EnsureWidgetGuid(BP, Placeholder);

	// Back button
	UWidget* BackBtn = CreateButton(BP, FName(TEXT("Button_Back")), FText::FromString(TEXT("Back")), 150.f, 50.f);
	VB_Main->AddChildToVerticalBox(BackBtn);

	if (UVerticalBoxSlot* BackSlot = Cast<UVerticalBoxSlot>(BackBtn->Slot))
	{
		BackSlot->SetHorizontalAlignment(HAlign_Left);
	}

	SaveScreen(BP, PATH_ACHIEVEMENTS);
}

void UT66ScreenSetupCommandlet::SetupAccountStatus()
{
	UObject* Loaded = UEditorAssetLibrary::LoadAsset(PATH_ACCOUNTSTATUS);
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
	if (!BP)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66ScreenSetup] Failed to load: %s"), PATH_ACCOUNTSTATUS);
		return;
	}

	UVerticalBox* VB_Main = CreateScreenBase(BP, TEXT("Account Status"));

	// Placeholder
	UTextBlock* Placeholder = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Placeholder")));
	Placeholder->SetText(FText::FromString(TEXT("[ Account Info - Coming Soon ]")));
	Placeholder->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.f)));
	FSlateFontInfo PlaceFont = Placeholder->GetFont();
	PlaceFont.Size = 16;
	Placeholder->SetFont(PlaceFont);
	VB_Main->AddChildToVerticalBox(Placeholder);

	if (UVerticalBoxSlot* PlaceSlot = Cast<UVerticalBoxSlot>(Placeholder->Slot))
	{
		PlaceSlot->SetHorizontalAlignment(HAlign_Center);
		PlaceSlot->SetPadding(FMargin(0.f, 80.f));
	}
	EnsureWidgetGuid(BP, Placeholder);

	// Back button
	UWidget* BackBtn = CreateButton(BP, FName(TEXT("Button_Back")), FText::FromString(TEXT("Back")), 150.f, 50.f);
	VB_Main->AddChildToVerticalBox(BackBtn);

	if (UVerticalBoxSlot* BackSlot = Cast<UVerticalBoxSlot>(BackBtn->Slot))
	{
		BackSlot->SetHorizontalAlignment(HAlign_Left);
	}

	SaveScreen(BP, PATH_ACCOUNTSTATUS);
}
