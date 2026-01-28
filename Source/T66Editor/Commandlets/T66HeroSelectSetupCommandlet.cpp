#include "T66HeroSelectSetupCommandlet.h"

#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScaleBox.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "EditorAssetLibrary.h"
#include "WidgetBlueprint.h"

static const TCHAR* HEROSELECT_PATH = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_HeroSelect_Solo.WBP_Screen_HeroSelect_Solo");
static const TCHAR* COMPANIONSELECT_PATH = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_CompanionSelect.WBP_Screen_CompanionSelect");

// Hero data
struct FHeroInfo
{
	FString Name;
	FLinearColor Color;
	FString Description;
};

static const FHeroInfo HEROES[] = {
	{TEXT("Warrior"), FLinearColor(0.8f, 0.2f, 0.2f, 1.0f), TEXT("A fierce melee combatant with high defense.")},
	{TEXT("Mage"), FLinearColor(0.2f, 0.2f, 0.9f, 1.0f), TEXT("A powerful spellcaster with devastating magic.")},
	{TEXT("Rogue"), FLinearColor(0.4f, 0.1f, 0.6f, 1.0f), TEXT("A swift assassin who strikes from the shadows.")},
	{TEXT("Ranger"), FLinearColor(0.2f, 0.7f, 0.2f, 1.0f), TEXT("A skilled archer with deadly precision.")},
	{TEXT("Paladin"), FLinearColor(0.9f, 0.85f, 0.2f, 1.0f), TEXT("A holy knight with healing abilities.")},
	{TEXT("Berserker"), FLinearColor(0.9f, 0.5f, 0.1f, 1.0f), TEXT("A raging warrior who grows stronger in combat.")},
};

// Companion data  
struct FCompanionInfo
{
	FString Name;
	FLinearColor Color;
	FString Description;
};

static const FCompanionInfo COMPANIONS[] = {
	{TEXT("Lumina"), FLinearColor(0.9f, 0.9f, 0.5f, 1.0f), TEXT("She tells me... of a time when the light was all there was.")},
	{TEXT("Umbra"), FLinearColor(0.3f, 0.3f, 0.4f, 1.0f), TEXT("She tells me... secrets whispered in the darkness.")},
	{TEXT("Ember"), FLinearColor(1.0f, 0.4f, 0.1f, 1.0f), TEXT("She tells me... of fires that never die.")},
	{TEXT("Crystal"), FLinearColor(0.5f, 0.8f, 1.0f, 1.0f), TEXT("She tells me... of frozen dreams beneath the ice.")},
};

namespace
{
	void EnsureWidgetGuid(UWidgetBlueprint* BP, UWidget* Widget)
	{
		if (!BP || !Widget) return;
		FGuid& Guid = BP->WidgetVariableNameToGuidMap.FindOrAdd(Widget->GetFName());
		if (!Guid.IsValid())
		{
			Guid = FGuid::NewGuid();
		}
	}

	UButton* CreateArrowButton(UWidgetBlueprint* BP, const FName& Name, bool bIsLeft)
	{
		UButton* Btn = BP->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
		FButtonStyle Style = Btn->GetStyle();
		Style.Normal.TintColor = FSlateColor(FLinearColor(0.3f, 0.3f, 0.4f, 0.8f));
		Style.Hovered.TintColor = FSlateColor(FLinearColor(0.4f, 0.4f, 0.5f, 1.0f));
		Style.Pressed.TintColor = FSlateColor(FLinearColor(0.2f, 0.2f, 0.3f, 1.0f));
		Btn->SetStyle(Style);

		UTextBlock* Arrow = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), 
			FName(*FString::Printf(TEXT("Txt_%s"), *Name.ToString())));
		Arrow->SetText(FText::FromString(bIsLeft ? TEXT("<") : TEXT(">")));
		FSlateFontInfo Font = Arrow->GetFont();
		Font.Size = 48;
		Arrow->SetFont(Font);
		Arrow->SetJustification(ETextJustify::Center);
		Btn->AddChild(Arrow);
		
		EnsureWidgetGuid(BP, Btn);
		EnsureWidgetGuid(BP, Arrow);
		return Btn;
	}

	UWidget* CreateActionButton(UWidgetBlueprint* BP, const FName& Name, const FText& Label, float Width, float Height, FLinearColor BgColor = FLinearColor(0.2f, 0.2f, 0.3f, 1.0f))
	{
		USizeBox* SizeBox = BP->WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), 
			FName(*FString::Printf(TEXT("SB_%s"), *Name.ToString())));
		SizeBox->SetWidthOverride(Width);
		SizeBox->SetHeightOverride(Height);

		UButton* Button = BP->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
		FButtonStyle Style = Button->GetStyle();
		Style.Normal.TintColor = FSlateColor(BgColor);
		Style.Hovered.TintColor = FSlateColor(BgColor * 1.2f);
		Style.Pressed.TintColor = FSlateColor(BgColor * 0.8f);
		Button->SetStyle(Style);
		SizeBox->AddChild(Button);

		UOverlay* Overlay = BP->WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), 
			FName(*FString::Printf(TEXT("Ov_%s"), *Name.ToString())));
		Button->AddChild(Overlay);

		UTextBlock* Text = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), 
			FName(*FString::Printf(TEXT("Txt_%s"), *Name.ToString())));
		Text->SetText(Label);
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = 14;
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

	UWidget* CreateHeroBeltButton(UWidgetBlueprint* BP, int32 Index, const FHeroInfo& Hero)
	{
		FString BtnName = FString::Printf(TEXT("Btn_Hero_%d"), Index);
		
		USizeBox* SizeBox = BP->WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), 
			FName(*FString::Printf(TEXT("SB_Hero_%d"), Index)));
		SizeBox->SetWidthOverride(50);
		SizeBox->SetHeightOverride(50);

		UButton* Btn = BP->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(*BtnName));
		FButtonStyle Style = Btn->GetStyle();
		Style.Normal.TintColor = FSlateColor(Hero.Color);
		Style.Hovered.TintColor = FSlateColor(Hero.Color * 1.3f);
		Style.Pressed.TintColor = FSlateColor(Hero.Color * 0.7f);
		Btn->SetStyle(Style);
		SizeBox->AddChild(Btn);

		UTextBlock* Initial = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), 
			FName(*FString::Printf(TEXT("Txt_Hero_%d"), Index)));
		Initial->SetText(FText::FromString(Hero.Name.Left(1)));
		FSlateFontInfo Font = Initial->GetFont();
		Font.Size = 20;
		Initial->SetFont(Font);
		Initial->SetJustification(ETextJustify::Center);
		Btn->AddChild(Initial);

		EnsureWidgetGuid(BP, SizeBox);
		EnsureWidgetGuid(BP, Btn);
		EnsureWidgetGuid(BP, Initial);

		return SizeBox;
	}
}

UT66HeroSelectSetupCommandlet::UT66HeroSelectSetupCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66HeroSelectSetupCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Display, TEXT("[T66HeroSelectSetup] Starting..."));

	SetupHeroSelectSolo();
	SetupCompanionSelect();

	UE_LOG(LogTemp, Display, TEXT("[T66HeroSelectSetup] Complete!"));
	return 0;
}

void UT66HeroSelectSetupCommandlet::SetupHeroSelectSolo()
{
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(UEditorAssetLibrary::LoadAsset(HEROSELECT_PATH));
	if (!BP)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66HeroSelectSetup] Could not load: %s"), HEROSELECT_PATH);
		return;
	}

	BP->Modify();
	BP->WidgetVariableNameToGuidMap.Empty();

	UWidgetTree* WT = NewObject<UWidgetTree>(BP, UWidgetTree::StaticClass(), NAME_None, RF_Transactional);

	// ========== ROOT CANVAS ==========
	UCanvasPanel* Root = WT->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName(TEXT("Root")));
	WT->RootWidget = Root;
	EnsureWidgetGuid(BP, Root);

	// Opaque background
	UBorder* Background = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Background")));
	Background->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.08f, 1.0f));
	Root->AddChild(Background);
	if (UCanvasPanelSlot* S = Cast<UCanvasPanelSlot>(Background->Slot))
	{
		S->SetAnchors(FAnchors(0, 0, 1, 1));
		S->SetOffsets(FMargin(0));
	}
	EnsureWidgetGuid(BP, Background);

	// ========== MAIN VERTICAL LAYOUT ==========
	UVerticalBox* MainVB = WT->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Main")));
	Root->AddChild(MainVB);
	if (UCanvasPanelSlot* S = Cast<UCanvasPanelSlot>(MainVB->Slot))
	{
		S->SetAnchors(FAnchors(0, 0, 1, 1));
		S->SetOffsets(FMargin(0));
	}
	EnsureWidgetGuid(BP, MainVB);

	// ========== TOP: TITLE + HERO BELT ==========
	UVerticalBox* TopSection = WT->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Top")));
	MainVB->AddChild(TopSection);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(TopSection->Slot))
	{
		S->SetPadding(FMargin(20, 15, 20, 10));
	}
	EnsureWidgetGuid(BP, TopSection);

	// Title
	UTextBlock* Title = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Title")));
	Title->SetText(FText::FromString(TEXT("HERO SELECTION")));
	FSlateFontInfo TitleFont = Title->GetFont();
	TitleFont.Size = 28;
	Title->SetFont(TitleFont);
	Title->SetJustification(ETextJustify::Center);
	TopSection->AddChild(Title);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(Title->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetPadding(FMargin(0, 0, 0, 8));
	}
	EnsureWidgetGuid(BP, Title);

	// Hero belt row
	UHorizontalBox* BeltRow = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_Belt")));
	TopSection->AddChild(BeltRow);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(BeltRow->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Center);
	}
	EnsureWidgetGuid(BP, BeltRow);

	// HERO GRID button
	UWidget* GridBtn = CreateActionButton(BP, FName(TEXT("Btn_HeroGrid")), FText::FromString(TEXT("HERO GRID")), 100, 35, FLinearColor(0.25f, 0.25f, 0.35f, 1.0f));
	BeltRow->AddChild(GridBtn);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(GridBtn->Slot))
	{
		S->SetPadding(FMargin(0, 0, 20, 0));
		S->SetVerticalAlignment(VAlign_Center);
	}

	// Hero icon buttons
	for (int32 i = 0; i < 6; ++i)
	{
		UWidget* HeroBtn = CreateHeroBeltButton(BP, i, HEROES[i]);
		BeltRow->AddChild(HeroBtn);
		if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(HeroBtn->Slot))
		{
			S->SetPadding(FMargin(4, 0, 4, 0));
			S->SetVerticalAlignment(VAlign_Center);
		}
	}

	// ========== CENTER: THREE-PANEL LAYOUT ==========
	UHorizontalBox* CenterHB = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_Center")));
	MainVB->AddChild(CenterHB);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(CenterHB->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 1.0f;
		S->SetSize(Fill);
		S->SetPadding(FMargin(20, 10, 20, 10));
	}
	EnsureWidgetGuid(BP, CenterHB);

	// ========== LEFT PANEL: SKINS ==========
	UBorder* LeftPanel = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_LeftPanel")));
	LeftPanel->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.12f, 0.9f));
	LeftPanel->SetPadding(FMargin(15));
	CenterHB->AddChild(LeftPanel);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(LeftPanel->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 0.22f;
		S->SetSize(Fill);
		S->SetPadding(FMargin(0, 0, 10, 0));
	}
	EnsureWidgetGuid(BP, LeftPanel);

	UVerticalBox* SkinsVB = WT->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Skins")));
	LeftPanel->AddChild(SkinsVB);
	EnsureWidgetGuid(BP, SkinsVB);

	UTextBlock* SkinsTitle = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_SkinsTitle")));
	SkinsTitle->SetText(FText::FromString(TEXT("SKINS")));
	FSlateFontInfo SkinsFont = SkinsTitle->GetFont();
	SkinsFont.Size = 18;
	SkinsTitle->SetFont(SkinsFont);
	SkinsVB->AddChild(SkinsTitle);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(SkinsTitle->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 10));
	}
	EnsureWidgetGuid(BP, SkinsTitle);

	// Placeholder skin rows
	for (int32 i = 0; i < 3; ++i)
	{
		UBorder* SkinRow = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), 
			FName(*FString::Printf(TEXT("Border_Skin_%d"), i)));
		SkinRow->SetBrushColor(FLinearColor(0.12f, 0.12f, 0.18f, 1.0f));
		SkinRow->SetPadding(FMargin(8));
		SkinsVB->AddChild(SkinRow);
		if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(SkinRow->Slot))
		{
			S->SetPadding(FMargin(0, 0, 0, 5));
		}
		EnsureWidgetGuid(BP, SkinRow);

		UHorizontalBox* SkinHB = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), 
			FName(*FString::Printf(TEXT("HB_Skin_%d"), i)));
		SkinRow->AddChild(SkinHB);
		EnsureWidgetGuid(BP, SkinHB);

		UTextBlock* SkinName = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), 
			FName(*FString::Printf(TEXT("Txt_SkinName_%d"), i)));
		SkinName->SetText(FText::FromString(i == 0 ? TEXT("Default") : FString::Printf(TEXT("Skin %d"), i)));
		FSlateFontInfo SkinFont = SkinName->GetFont();
		SkinFont.Size = 12;
		SkinName->SetFont(SkinFont);
		SkinHB->AddChild(SkinName);
		if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(SkinName->Slot))
		{
			FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 1.0f;
			S->SetSize(Fill);
			S->SetVerticalAlignment(VAlign_Center);
		}
		EnsureWidgetGuid(BP, SkinName);

		UWidget* EquipBtn = CreateActionButton(BP, FName(*FString::Printf(TEXT("Btn_Equip_%d"), i)), 
			FText::FromString(i == 0 ? TEXT("EQUIPPED") : TEXT("EQUIP")), 70, 25, 
			i == 0 ? FLinearColor(0.2f, 0.5f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.4f, 1.0f));
		SkinHB->AddChild(EquipBtn);
	}

	// ========== CENTER PANEL: PREVIEW WITH ARROWS ==========
	UOverlay* CenterPanel = WT->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName(TEXT("Overlay_Preview")));
	CenterHB->AddChild(CenterPanel);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(CenterPanel->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 0.56f;
		S->SetSize(Fill);
	}
	EnsureWidgetGuid(BP, CenterPanel);

	// Preview background
	UBorder* PreviewBg = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_Preview")));
	PreviewBg->SetBrushColor(FLinearColor(0.1f, 0.1f, 0.15f, 0.95f));
	CenterPanel->AddChild(PreviewBg);
	if (UOverlaySlot* S = Cast<UOverlaySlot>(PreviewBg->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Fill);
		S->SetVerticalAlignment(VAlign_Fill);
	}
	EnsureWidgetGuid(BP, PreviewBg);

	// Preview content layout (hero cylinder + companion cube in center)
	UVerticalBox* PreviewVB = WT->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Preview")));
	CenterPanel->AddChild(PreviewVB);
	if (UOverlaySlot* S = Cast<UOverlaySlot>(PreviewVB->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Fill);
		S->SetVerticalAlignment(VAlign_Fill);
		S->SetPadding(FMargin(20));
	}
	EnsureWidgetGuid(BP, PreviewVB);

	// Preview area with colored shape
	UHorizontalBox* PreviewArea = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_PreviewArea")));
	PreviewVB->AddChild(PreviewArea);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(PreviewArea->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 1.0f;
		S->SetSize(Fill);
	}
	EnsureWidgetGuid(BP, PreviewArea);

	// Left arrow button
	USizeBox* LeftArrowSB = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(TEXT("SB_LeftArrow")));
	LeftArrowSB->SetWidthOverride(60);
	PreviewArea->AddChild(LeftArrowSB);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(LeftArrowSB->Slot))
	{
		S->SetVerticalAlignment(VAlign_Center);
	}
	EnsureWidgetGuid(BP, LeftArrowSB);

	UButton* LeftArrow = CreateArrowButton(BP, FName(TEXT("Btn_PrevHero")), true);
	LeftArrowSB->AddChild(LeftArrow);

	// Center preview - 3D rendered character preview
	UOverlay* HeroPreview = WT->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName(TEXT("Overlay_HeroPreview")));
	PreviewArea->AddChild(HeroPreview);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(HeroPreview->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 1.0f;
		S->SetSize(Fill);
	}
	EnsureWidgetGuid(BP, HeroPreview);

	// 3D Preview Image - displays render target from SceneCapture
	UImage* HeroPreviewImage = WT->ConstructWidget<UImage>(UImage::StaticClass(), FName(TEXT("Img_HeroPreview")));
	HeroPreviewImage->SetColorAndOpacity(FLinearColor::White);
	HeroPreview->AddChild(HeroPreviewImage);
	if (UOverlaySlot* S = Cast<UOverlaySlot>(HeroPreviewImage->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetVerticalAlignment(VAlign_Center);
		S->SetPadding(FMargin(0, 0, 50, 0)); // Leave room for companion
	}
	EnsureWidgetGuid(BP, HeroPreviewImage);

	// Fallback: Hero cylinder visual (shown if render target not available)
	UBorder* HeroCylinder = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_HeroCylinder")));
	HeroCylinder->SetBrushColor(HEROES[0].Color);
	HeroCylinder->SetVisibility(ESlateVisibility::Collapsed); // Hidden by default, shown as fallback
	HeroPreview->AddChild(HeroCylinder);
	if (UOverlaySlot* S = Cast<UOverlaySlot>(HeroCylinder->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetVerticalAlignment(VAlign_Center);
		S->SetPadding(FMargin(50, 20, 100, 20));
	}
	EnsureWidgetGuid(BP, HeroCylinder);

	// Inner cylinder content
	USizeBox* CylinderSize = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(TEXT("SB_Cylinder")));
	CylinderSize->SetWidthOverride(100);
	CylinderSize->SetHeightOverride(200);
	HeroCylinder->AddChild(CylinderSize);
	EnsureWidgetGuid(BP, CylinderSize);

	UBorder* CylinderInner = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_CylinderInner")));
	CylinderInner->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.3f));
	CylinderSize->AddChild(CylinderInner);
	EnsureWidgetGuid(BP, CylinderInner);

	// Companion cube (smaller, positioned to the side)
	UBorder* CompanionCube = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_CompanionCube")));
	CompanionCube->SetBrushColor(COMPANIONS[0].Color);
	HeroPreview->AddChild(CompanionCube);
	if (UOverlaySlot* S = Cast<UOverlaySlot>(CompanionCube->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Right);
		S->SetVerticalAlignment(VAlign_Bottom);
		S->SetPadding(FMargin(0, 0, 40, 40));
	}
	EnsureWidgetGuid(BP, CompanionCube);

	USizeBox* CubeSize = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(TEXT("SB_CompanionCube")));
	CubeSize->SetWidthOverride(60);
	CubeSize->SetHeightOverride(60);
	CompanionCube->AddChild(CubeSize);
	EnsureWidgetGuid(BP, CubeSize);

	UBorder* CubeInner = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_CubeInner")));
	CubeInner->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.3f));
	CubeSize->AddChild(CubeInner);
	EnsureWidgetGuid(BP, CubeInner);

	// CHOOSE COMPANION button above companion
	UWidget* ChooseCompBtn = CreateActionButton(BP, FName(TEXT("Btn_ChooseCompanion")), FText::FromString(TEXT("CHOOSE COMPANION")), 140, 30, FLinearColor(0.3f, 0.3f, 0.5f, 1.0f));
	HeroPreview->AddChild(ChooseCompBtn);
	if (UOverlaySlot* S = Cast<UOverlaySlot>(ChooseCompBtn->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Right);
		S->SetVerticalAlignment(VAlign_Bottom);
		S->SetPadding(FMargin(0, 0, 10, 110));
	}

	// Right arrow button
	USizeBox* RightArrowSB = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(TEXT("SB_RightArrow")));
	RightArrowSB->SetWidthOverride(60);
	PreviewArea->AddChild(RightArrowSB);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(RightArrowSB->Slot))
	{
		S->SetVerticalAlignment(VAlign_Center);
	}
	EnsureWidgetGuid(BP, RightArrowSB);

	UButton* RightArrow = CreateArrowButton(BP, FName(TEXT("Btn_NextHero")), false);
	RightArrowSB->AddChild(RightArrow);

	// ========== RIGHT PANEL: HERO INFO ==========
	UBorder* RightPanel = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_RightPanel")));
	RightPanel->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.12f, 0.9f));
	RightPanel->SetPadding(FMargin(15));
	CenterHB->AddChild(RightPanel);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(RightPanel->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 0.22f;
		S->SetSize(Fill);
		S->SetPadding(FMargin(10, 0, 0, 0));
	}
	EnsureWidgetGuid(BP, RightPanel);

	UVerticalBox* InfoVB = WT->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_HeroInfo")));
	RightPanel->AddChild(InfoVB);
	EnsureWidgetGuid(BP, InfoVB);

	// Hero name
	UTextBlock* HeroName = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_HeroName")));
	HeroName->SetText(FText::FromString(TEXT("WARRIOR")));
	HeroName->SetColorAndOpacity(FSlateColor(HEROES[0].Color));
	FSlateFontInfo NameFont = HeroName->GetFont();
	NameFont.Size = 26;
	HeroName->SetFont(NameFont);
	HeroName->SetJustification(ETextJustify::Center);
	InfoVB->AddChild(HeroName);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(HeroName->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 15));
	}
	EnsureWidgetGuid(BP, HeroName);

	// Video preview placeholder
	UBorder* VideoPlaceholder = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_VideoPreview")));
	VideoPlaceholder->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.08f, 1.0f));
	InfoVB->AddChild(VideoPlaceholder);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(VideoPlaceholder->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 10));
	}
	EnsureWidgetGuid(BP, VideoPlaceholder);

	USizeBox* VideoSize = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(TEXT("SB_Video")));
	VideoSize->SetHeightOverride(100);
	VideoPlaceholder->AddChild(VideoSize);
	EnsureWidgetGuid(BP, VideoSize);

	UTextBlock* VideoText = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_VideoPlaceholder")));
	VideoText->SetText(FText::FromString(TEXT("[Gameplay Video]")));
	VideoText->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.4f, 0.4f, 1.0f)));
	VideoText->SetJustification(ETextJustify::Center);
	VideoSize->AddChild(VideoText);
	EnsureWidgetGuid(BP, VideoText);

	// Hero description
	UTextBlock* HeroDesc = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_HeroDesc")));
	HeroDesc->SetText(FText::FromString(HEROES[0].Description));
	FSlateFontInfo DescFont = HeroDesc->GetFont();
	DescFont.Size = 12;
	HeroDesc->SetFont(DescFont);
	HeroDesc->SetAutoWrapText(true);
	InfoVB->AddChild(HeroDesc);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(HeroDesc->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 15));
	}
	EnsureWidgetGuid(BP, HeroDesc);

	// Medals row
	UHorizontalBox* MedalsHB = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_Medals")));
	InfoVB->AddChild(MedalsHB);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(MedalsHB->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 15));
		S->SetHorizontalAlignment(HAlign_Center);
	}
	EnsureWidgetGuid(BP, MedalsHB);

	// Medal placeholders (Black, Red, Yellow, White)
	FLinearColor MedalColors[] = {FLinearColor(0.2f, 0.2f, 0.2f, 0.5f), FLinearColor(0.5f, 0.2f, 0.2f, 0.5f), 
								  FLinearColor(0.5f, 0.5f, 0.2f, 0.5f), FLinearColor(0.5f, 0.5f, 0.5f, 0.5f)};
	for (int32 i = 0; i < 4; ++i)
	{
		UBorder* Medal = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), 
			FName(*FString::Printf(TEXT("Border_Medal_%d"), i)));
		Medal->SetBrushColor(MedalColors[i]);
		MedalsHB->AddChild(Medal);
		if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(Medal->Slot))
		{
			S->SetPadding(FMargin(3, 0, 3, 0));
		}
		EnsureWidgetGuid(BP, Medal);

		USizeBox* MedalSize = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), 
			FName(*FString::Printf(TEXT("SB_Medal_%d"), i)));
		MedalSize->SetWidthOverride(25);
		MedalSize->SetHeightOverride(25);
		Medal->AddChild(MedalSize);
		EnsureWidgetGuid(BP, MedalSize);
	}

	// Lore button
	UWidget* LoreBtn = CreateActionButton(BP, FName(TEXT("Btn_Lore")), FText::FromString(TEXT("LORE")), 60, 25, FLinearColor(0.25f, 0.25f, 0.35f, 1.0f));
	InfoVB->AddChild(LoreBtn);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(LoreBtn->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Right);
		S->SetPadding(FMargin(0, 0, 0, 15));
	}

	// Difficulty selector
	UTextBlock* DiffLabel = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_DiffLabel")));
	DiffLabel->SetText(FText::FromString(TEXT("DIFFICULTY")));
	FSlateFontInfo DiffFont = DiffLabel->GetFont();
	DiffFont.Size = 12;
	DiffLabel->SetFont(DiffFont);
	InfoVB->AddChild(DiffLabel);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(DiffLabel->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 5));
	}
	EnsureWidgetGuid(BP, DiffLabel);

	UWidget* DiffBtn = CreateActionButton(BP, FName(TEXT("Btn_Difficulty")), FText::FromString(TEXT("EASY")), 100, 30, FLinearColor(0.2f, 0.4f, 0.2f, 1.0f));
	InfoVB->AddChild(DiffBtn);

	// ========== BOTTOM: ACTION BUTTONS ==========
	UHorizontalBox* BottomHB = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_Bottom")));
	MainVB->AddChild(BottomHB);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(BottomHB->Slot))
	{
		S->SetPadding(FMargin(20, 10, 20, 20));
		S->SetHorizontalAlignment(HAlign_Center);
	}
	EnsureWidgetGuid(BP, BottomHB);

	// Back button
	UWidget* BackBtn = CreateActionButton(BP, FName(TEXT("Btn_Back")), FText::FromString(TEXT("Back")), 100, 40);
	BottomHB->AddChild(BackBtn);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(BackBtn->Slot))
	{
		S->SetPadding(FMargin(0, 0, 15, 0));
	}

	// THE LAB button
	UWidget* LabBtn = CreateActionButton(BP, FName(TEXT("Btn_TheLab")), FText::FromString(TEXT("THE LAB")), 100, 40, FLinearColor(0.3f, 0.3f, 0.4f, 1.0f));
	BottomHB->AddChild(LabBtn);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(LabBtn->Slot))
	{
		S->SetPadding(FMargin(0, 0, 15, 0));
	}

	// ENTER THE TRIBULATION button
	UWidget* EnterBtn = CreateActionButton(BP, FName(TEXT("Btn_EnterTribulation")), FText::FromString(TEXT("ENTER THE TRIBULATION")), 200, 45, FLinearColor(0.6f, 0.2f, 0.2f, 1.0f));
	BottomHB->AddChild(EnterBtn);

	// Assign widget tree
	BP->WidgetTree = WT;

	bool bSaved = UEditorAssetLibrary::SaveAsset(HEROSELECT_PATH, false);
	UE_LOG(LogTemp, Display, TEXT("[T66HeroSelectSetup] Hero Select Solo saved: %s"), bSaved ? TEXT("YES") : TEXT("NO"));
}

void UT66HeroSelectSetupCommandlet::SetupCompanionSelect()
{
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(UEditorAssetLibrary::LoadAsset(COMPANIONSELECT_PATH));
	if (!BP)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66HeroSelectSetup] Companion select doesn't exist, skipping"));
		return;
	}

	BP->Modify();
	BP->WidgetVariableNameToGuidMap.Empty();

	UWidgetTree* WT = NewObject<UWidgetTree>(BP, UWidgetTree::StaticClass(), NAME_None, RF_Transactional);

	// ========== ROOT CANVAS ==========
	UCanvasPanel* Root = WT->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName(TEXT("Root")));
	WT->RootWidget = Root;
	EnsureWidgetGuid(BP, Root);

	// Opaque background
	UBorder* Background = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Background")));
	Background->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.08f, 1.0f));
	Root->AddChild(Background);
	if (UCanvasPanelSlot* S = Cast<UCanvasPanelSlot>(Background->Slot))
	{
		S->SetAnchors(FAnchors(0, 0, 1, 1));
		S->SetOffsets(FMargin(0));
	}
	EnsureWidgetGuid(BP, Background);

	// ========== MAIN VERTICAL LAYOUT ==========
	UVerticalBox* MainVB = WT->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Main")));
	Root->AddChild(MainVB);
	if (UCanvasPanelSlot* S = Cast<UCanvasPanelSlot>(MainVB->Slot))
	{
		S->SetAnchors(FAnchors(0, 0, 1, 1));
		S->SetOffsets(FMargin(0));
	}
	EnsureWidgetGuid(BP, MainVB);

	// ========== TOP: TITLE + COMPANION BELT ==========
	UVerticalBox* TopSection = WT->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Top")));
	MainVB->AddChild(TopSection);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(TopSection->Slot))
	{
		S->SetPadding(FMargin(20, 15, 20, 10));
	}
	EnsureWidgetGuid(BP, TopSection);

	// Title
	UTextBlock* Title = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Title")));
	Title->SetText(FText::FromString(TEXT("COMPANION SELECTION")));
	FSlateFontInfo TitleFont = Title->GetFont();
	TitleFont.Size = 28;
	Title->SetFont(TitleFont);
	Title->SetJustification(ETextJustify::Center);
	TopSection->AddChild(Title);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(Title->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetPadding(FMargin(0, 0, 0, 8));
	}
	EnsureWidgetGuid(BP, Title);

	// Companion belt row
	UHorizontalBox* BeltRow = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_Belt")));
	TopSection->AddChild(BeltRow);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(BeltRow->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Center);
	}
	EnsureWidgetGuid(BP, BeltRow);

	// NO COMPANION button
	USizeBox* NoCompSB = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(TEXT("SB_NoCompanion")));
	NoCompSB->SetWidthOverride(50);
	NoCompSB->SetHeightOverride(50);
	BeltRow->AddChild(NoCompSB);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(NoCompSB->Slot))
	{
		S->SetPadding(FMargin(0, 0, 15, 0));
		S->SetVerticalAlignment(VAlign_Center);
	}
	EnsureWidgetGuid(BP, NoCompSB);

	UButton* NoCompBtn = WT->ConstructWidget<UButton>(UButton::StaticClass(), FName(TEXT("Btn_NoCompanion")));
	FButtonStyle NoCompStyle = NoCompBtn->GetStyle();
	NoCompStyle.Normal.TintColor = FSlateColor(FLinearColor(0.4f, 0.2f, 0.2f, 1.0f));
	NoCompStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.5f, 0.25f, 0.25f, 1.0f));
	NoCompBtn->SetStyle(NoCompStyle);
	NoCompSB->AddChild(NoCompBtn);
	EnsureWidgetGuid(BP, NoCompBtn);

	UTextBlock* NoCompText = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Txt_NoCompanion")));
	NoCompText->SetText(FText::FromString(TEXT("X")));
	NoCompText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 0.5f, 0.5f, 1)));
	FSlateFontInfo NoCompFont = NoCompText->GetFont();
	NoCompFont.Size = 24;
	NoCompText->SetFont(NoCompFont);
	NoCompText->SetJustification(ETextJustify::Center);
	NoCompBtn->AddChild(NoCompText);
	EnsureWidgetGuid(BP, NoCompText);

	// Companion icon buttons
	for (int32 i = 0; i < 4; ++i)
	{
		FString BtnName = FString::Printf(TEXT("Btn_Companion_%d"), i);
		
		USizeBox* SB = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), 
			FName(*FString::Printf(TEXT("SB_Companion_%d"), i)));
		SB->SetWidthOverride(50);
		SB->SetHeightOverride(50);
		BeltRow->AddChild(SB);
		if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(SB->Slot))
		{
			S->SetPadding(FMargin(4, 0, 4, 0));
			S->SetVerticalAlignment(VAlign_Center);
		}
		EnsureWidgetGuid(BP, SB);

		UButton* Btn = WT->ConstructWidget<UButton>(UButton::StaticClass(), FName(*BtnName));
		FButtonStyle Style = Btn->GetStyle();
		Style.Normal.TintColor = FSlateColor(COMPANIONS[i].Color);
		Style.Hovered.TintColor = FSlateColor(COMPANIONS[i].Color * 1.3f);
		Btn->SetStyle(Style);
		SB->AddChild(Btn);
		EnsureWidgetGuid(BP, Btn);

		UTextBlock* Initial = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), 
			FName(*FString::Printf(TEXT("Txt_Companion_%d"), i)));
		Initial->SetText(FText::FromString(COMPANIONS[i].Name.Left(1)));
		FSlateFontInfo Font = Initial->GetFont();
		Font.Size = 20;
		Initial->SetFont(Font);
		Initial->SetJustification(ETextJustify::Center);
		Btn->AddChild(Initial);
		EnsureWidgetGuid(BP, Initial);
	}

	// ========== CENTER: THREE-PANEL LAYOUT ==========
	UHorizontalBox* CenterHB = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_Center")));
	MainVB->AddChild(CenterHB);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(CenterHB->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 1.0f;
		S->SetSize(Fill);
		S->SetPadding(FMargin(20, 10, 20, 10));
	}
	EnsureWidgetGuid(BP, CenterHB);

	// ========== LEFT PANEL: SKINS ==========
	UBorder* LeftPanel = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_LeftPanel")));
	LeftPanel->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.12f, 0.9f));
	LeftPanel->SetPadding(FMargin(15));
	CenterHB->AddChild(LeftPanel);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(LeftPanel->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 0.22f;
		S->SetSize(Fill);
		S->SetPadding(FMargin(0, 0, 10, 0));
	}
	EnsureWidgetGuid(BP, LeftPanel);

	UVerticalBox* SkinsVB = WT->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Skins")));
	LeftPanel->AddChild(SkinsVB);
	EnsureWidgetGuid(BP, SkinsVB);

	UTextBlock* SkinsTitle = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_SkinsTitle")));
	SkinsTitle->SetText(FText::FromString(TEXT("SKINS")));
	FSlateFontInfo SkinsFont = SkinsTitle->GetFont();
	SkinsFont.Size = 18;
	SkinsTitle->SetFont(SkinsFont);
	SkinsVB->AddChild(SkinsTitle);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(SkinsTitle->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 10));
	}
	EnsureWidgetGuid(BP, SkinsTitle);

	// Placeholder skin rows
	for (int32 i = 0; i < 2; ++i)
	{
		UBorder* SkinRow = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), 
			FName(*FString::Printf(TEXT("Border_Skin_%d"), i)));
		SkinRow->SetBrushColor(FLinearColor(0.12f, 0.12f, 0.18f, 1.0f));
		SkinRow->SetPadding(FMargin(8));
		SkinsVB->AddChild(SkinRow);
		if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(SkinRow->Slot))
		{
			S->SetPadding(FMargin(0, 0, 0, 5));
		}
		EnsureWidgetGuid(BP, SkinRow);

		UHorizontalBox* SkinHB = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), 
			FName(*FString::Printf(TEXT("HB_Skin_%d"), i)));
		SkinRow->AddChild(SkinHB);
		EnsureWidgetGuid(BP, SkinHB);

		UTextBlock* SkinName = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), 
			FName(*FString::Printf(TEXT("Txt_SkinName_%d"), i)));
		SkinName->SetText(FText::FromString(i == 0 ? TEXT("Default") : TEXT("Pet Form")));
		FSlateFontInfo SkinFont = SkinName->GetFont();
		SkinFont.Size = 12;
		SkinName->SetFont(SkinFont);
		SkinHB->AddChild(SkinName);
		if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(SkinName->Slot))
		{
			FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 1.0f;
			S->SetSize(Fill);
			S->SetVerticalAlignment(VAlign_Center);
		}
		EnsureWidgetGuid(BP, SkinName);

		UWidget* EquipBtn = CreateActionButton(BP, FName(*FString::Printf(TEXT("Btn_CEquip_%d"), i)), 
			FText::FromString(i == 0 ? TEXT("EQUIPPED") : TEXT("EQUIP")), 70, 25, 
			i == 0 ? FLinearColor(0.2f, 0.5f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.4f, 1.0f));
		SkinHB->AddChild(EquipBtn);
	}

	// ========== CENTER PANEL: PREVIEW ==========
	UOverlay* CenterPanel = WT->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName(TEXT("Overlay_Preview")));
	CenterHB->AddChild(CenterPanel);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(CenterPanel->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 0.56f;
		S->SetSize(Fill);
	}
	EnsureWidgetGuid(BP, CenterPanel);

	UBorder* PreviewBg = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_Preview")));
	PreviewBg->SetBrushColor(FLinearColor(0.1f, 0.1f, 0.15f, 0.95f));
	CenterPanel->AddChild(PreviewBg);
	if (UOverlaySlot* S = Cast<UOverlaySlot>(PreviewBg->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Fill);
		S->SetVerticalAlignment(VAlign_Fill);
	}
	EnsureWidgetGuid(BP, PreviewBg);

	UVerticalBox* PreviewVB = WT->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_Preview")));
	CenterPanel->AddChild(PreviewVB);
	if (UOverlaySlot* S = Cast<UOverlaySlot>(PreviewVB->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Fill);
		S->SetVerticalAlignment(VAlign_Fill);
		S->SetPadding(FMargin(20));
	}
	EnsureWidgetGuid(BP, PreviewVB);

	// Preview area with arrows
	UHorizontalBox* PreviewArea = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_PreviewArea")));
	PreviewVB->AddChild(PreviewArea);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(PreviewArea->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 1.0f;
		S->SetSize(Fill);
	}
	EnsureWidgetGuid(BP, PreviewArea);

	// Left arrow
	USizeBox* LeftArrowSB = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(TEXT("SB_LeftArrow")));
	LeftArrowSB->SetWidthOverride(60);
	PreviewArea->AddChild(LeftArrowSB);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(LeftArrowSB->Slot))
	{
		S->SetVerticalAlignment(VAlign_Center);
	}
	EnsureWidgetGuid(BP, LeftArrowSB);

	UButton* LeftArrow = CreateArrowButton(BP, FName(TEXT("Btn_PrevCompanion")), true);
	LeftArrowSB->AddChild(LeftArrow);

	// Companion preview - 3D rendered companion
	UOverlay* CompPreview = WT->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName(TEXT("Overlay_CompPreview")));
	PreviewArea->AddChild(CompPreview);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(CompPreview->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 1.0f;
		S->SetSize(Fill);
	}
	EnsureWidgetGuid(BP, CompPreview);

	// 3D Preview Image - displays render target from SceneCapture
	UImage* CompPreviewImage = WT->ConstructWidget<UImage>(UImage::StaticClass(), FName(TEXT("Img_CompanionPreview")));
	CompPreviewImage->SetColorAndOpacity(FLinearColor::White);
	CompPreview->AddChild(CompPreviewImage);
	if (UOverlaySlot* S = Cast<UOverlaySlot>(CompPreviewImage->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetVerticalAlignment(VAlign_Center);
	}
	EnsureWidgetGuid(BP, CompPreviewImage);

	// Fallback: Companion cube visual (shown if render target not available)
	UBorder* CompCube = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_CompanionCube")));
	CompCube->SetBrushColor(COMPANIONS[0].Color);
	CompCube->SetVisibility(ESlateVisibility::Collapsed); // Hidden by default
	CompPreview->AddChild(CompCube);
	if (UOverlaySlot* S = Cast<UOverlaySlot>(CompCube->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetVerticalAlignment(VAlign_Center);
	}
	EnsureWidgetGuid(BP, CompCube);

	USizeBox* CubeSize = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(TEXT("SB_Cube")));
	CubeSize->SetWidthOverride(120);
	CubeSize->SetHeightOverride(120);
	CompCube->AddChild(CubeSize);
	EnsureWidgetGuid(BP, CubeSize);

	UBorder* CubeInner = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_CubeInner")));
	CubeInner->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.3f));
	CubeSize->AddChild(CubeInner);
	EnsureWidgetGuid(BP, CubeInner);

	// Right arrow
	USizeBox* RightArrowSB = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(TEXT("SB_RightArrow")));
	RightArrowSB->SetWidthOverride(60);
	PreviewArea->AddChild(RightArrowSB);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(RightArrowSB->Slot))
	{
		S->SetVerticalAlignment(VAlign_Center);
	}
	EnsureWidgetGuid(BP, RightArrowSB);

	UButton* RightArrow = CreateArrowButton(BP, FName(TEXT("Btn_NextCompanion")), false);
	RightArrowSB->AddChild(RightArrow);

	// ========== RIGHT PANEL: COMPANION INFO ==========
	UBorder* RightPanel = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_RightPanel")));
	RightPanel->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.12f, 0.9f));
	RightPanel->SetPadding(FMargin(15));
	CenterHB->AddChild(RightPanel);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(RightPanel->Slot))
	{
		FSlateChildSize Fill; Fill.SizeRule = ESlateSizeRule::Fill; Fill.Value = 0.22f;
		S->SetSize(Fill);
		S->SetPadding(FMargin(10, 0, 0, 0));
	}
	EnsureWidgetGuid(BP, RightPanel);

	UVerticalBox* InfoVB = WT->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_CompanionInfo")));
	RightPanel->AddChild(InfoVB);
	EnsureWidgetGuid(BP, InfoVB);

	// Companion name
	UTextBlock* CompName = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_CompanionName")));
	CompName->SetText(FText::FromString(TEXT("LUMINA")));
	CompName->SetColorAndOpacity(FSlateColor(COMPANIONS[0].Color));
	FSlateFontInfo NameFont = CompName->GetFont();
	NameFont.Size = 26;
	CompName->SetFont(NameFont);
	CompName->SetJustification(ETextJustify::Center);
	InfoVB->AddChild(CompName);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(CompName->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 15));
	}
	EnsureWidgetGuid(BP, CompName);

	// Companion lore
	UTextBlock* CompLore = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_CompanionLore")));
	CompLore->SetText(FText::FromString(COMPANIONS[0].Description));
	CompLore->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)));
	FSlateFontInfo LoreFont = CompLore->GetFont();
	LoreFont.Size = 12;
	CompLore->SetFont(LoreFont);
	CompLore->SetAutoWrapText(true);
	InfoVB->AddChild(CompLore);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(CompLore->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 15));
	}
	EnsureWidgetGuid(BP, CompLore);

	// Passive effect
	UTextBlock* PassiveLabel = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_PassiveLabel")));
	PassiveLabel->SetText(FText::FromString(TEXT("PASSIVE EFFECT")));
	PassiveLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f)));
	FSlateFontInfo PassiveLabelFont = PassiveLabel->GetFont();
	PassiveLabelFont.Size = 10;
	PassiveLabel->SetFont(PassiveLabelFont);
	InfoVB->AddChild(PassiveLabel);
	EnsureWidgetGuid(BP, PassiveLabel);

	UTextBlock* PassiveText = WT->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Passive")));
	PassiveText->SetText(FText::FromString(TEXT("Heals the hero over time")));
	PassiveText->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 1.0f, 0.5f, 1.0f)));
	FSlateFontInfo PassiveFont = PassiveText->GetFont();
	PassiveFont.Size = 12;
	PassiveText->SetFont(PassiveFont);
	InfoVB->AddChild(PassiveText);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(PassiveText->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 15));
	}
	EnsureWidgetGuid(BP, PassiveText);

	// Medals (3 for companions)
	UHorizontalBox* MedalsHB = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_Medals")));
	InfoVB->AddChild(MedalsHB);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(MedalsHB->Slot))
	{
		S->SetPadding(FMargin(0, 0, 0, 15));
		S->SetHorizontalAlignment(HAlign_Center);
	}
	EnsureWidgetGuid(BP, MedalsHB);

	FLinearColor MedalColors[] = {FLinearColor(0.2f, 0.5f, 0.2f, 0.5f), FLinearColor(0.5f, 0.5f, 0.2f, 0.5f), 
								  FLinearColor(0.5f, 0.2f, 0.2f, 0.5f)};
	for (int32 i = 0; i < 3; ++i)
	{
		UBorder* Medal = WT->ConstructWidget<UBorder>(UBorder::StaticClass(), 
			FName(*FString::Printf(TEXT("Border_Medal_%d"), i)));
		Medal->SetBrushColor(MedalColors[i]);
		MedalsHB->AddChild(Medal);
		if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(Medal->Slot))
		{
			S->SetPadding(FMargin(3, 0, 3, 0));
		}
		EnsureWidgetGuid(BP, Medal);

		USizeBox* MedalSize = WT->ConstructWidget<USizeBox>(USizeBox::StaticClass(), 
			FName(*FString::Printf(TEXT("SB_Medal_%d"), i)));
		MedalSize->SetWidthOverride(25);
		MedalSize->SetHeightOverride(25);
		Medal->AddChild(MedalSize);
		EnsureWidgetGuid(BP, MedalSize);
	}

	// Lore button
	UWidget* LoreBtn = CreateActionButton(BP, FName(TEXT("Btn_Lore")), FText::FromString(TEXT("LORE")), 60, 25, FLinearColor(0.25f, 0.25f, 0.35f, 1.0f));
	InfoVB->AddChild(LoreBtn);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(LoreBtn->Slot))
	{
		S->SetHorizontalAlignment(HAlign_Right);
	}

	// ========== BOTTOM: ACTION BUTTONS ==========
	UHorizontalBox* BottomHB = WT->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(TEXT("HB_Bottom")));
	MainVB->AddChild(BottomHB);
	if (UVerticalBoxSlot* S = Cast<UVerticalBoxSlot>(BottomHB->Slot))
	{
		S->SetPadding(FMargin(20, 10, 20, 20));
		S->SetHorizontalAlignment(HAlign_Center);
	}
	EnsureWidgetGuid(BP, BottomHB);

	// Back button
	UWidget* BackBtn = CreateActionButton(BP, FName(TEXT("Btn_Back")), FText::FromString(TEXT("Back")), 100, 40);
	BottomHB->AddChild(BackBtn);
	if (UHorizontalBoxSlot* S = Cast<UHorizontalBoxSlot>(BackBtn->Slot))
	{
		S->SetPadding(FMargin(0, 0, 15, 0));
	}

	// CONFIRM COMPANION button
	UWidget* ConfirmBtn = CreateActionButton(BP, FName(TEXT("Btn_ConfirmCompanion")), FText::FromString(TEXT("CONFIRM COMPANION")), 180, 45, FLinearColor(0.2f, 0.5f, 0.2f, 1.0f));
	BottomHB->AddChild(ConfirmBtn);

	// Assign widget tree
	BP->WidgetTree = WT;

	bool bSaved = UEditorAssetLibrary::SaveAsset(COMPANIONSELECT_PATH, false);
	UE_LOG(LogTemp, Display, TEXT("[T66HeroSelectSetup] Companion Select saved: %s"), bSaved ? TEXT("YES") : TEXT("NO"));
}
