#include "T66MainMenuCenterCommandlet.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBoxSlot.h"
#include "Components/UniformGridSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "EditorAssetLibrary.h"
#include "WidgetBlueprint.h"

static const TCHAR* T66_MAINMENU_WBP_PATH = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_MainMenu.WBP_Screen_MainMenu");

namespace
{
	static bool CenterWidgetSlot(UWidget* Widget)
	{
		if (!Widget || !Widget->Slot)
		{
			return false;
		}

		bool bChanged = false;
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			FAnchors Anchors = CanvasSlot->GetAnchors();
			Anchors.Minimum.X = 0.5f;
			Anchors.Maximum.X = 0.5f;
			CanvasSlot->SetAnchors(Anchors);

			const FVector2D Alignment = CanvasSlot->GetAlignment();
			CanvasSlot->SetAlignment(FVector2D(0.5f, Alignment.Y));

			FMargin Offsets = CanvasSlot->GetOffsets();
			Offsets.Left = 0.f;
			CanvasSlot->SetOffsets(Offsets);
			bChanged = true;
		}
		else if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
		{
			OverlaySlot->SetHorizontalAlignment(HAlign_Center);
			bChanged = true;
		}
		else if (UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		{
			VerticalSlot->SetHorizontalAlignment(HAlign_Center);
			bChanged = true;
		}
		else if (UHorizontalBoxSlot* HorizontalSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		{
			HorizontalSlot->SetHorizontalAlignment(HAlign_Center);
			bChanged = true;
		}
		else if (UGridSlot* GridSlot = Cast<UGridSlot>(Widget->Slot))
		{
			GridSlot->SetHorizontalAlignment(HAlign_Center);
			bChanged = true;
		}
		else if (UUniformGridSlot* UniformSlot = Cast<UUniformGridSlot>(Widget->Slot))
		{
			UniformSlot->SetHorizontalAlignment(HAlign_Center);
			bChanged = true;
		}
		else if (USizeBoxSlot* SizeSlot = Cast<USizeBoxSlot>(Widget->Slot))
		{
			SizeSlot->SetHorizontalAlignment(HAlign_Center);
			bChanged = true;
		}

		return bChanged;
	}

	static bool StretchToFill(UWidget* Widget)
	{
		if (!Widget || !Widget->Slot)
		{
			return false;
		}

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			CanvasSlot->SetAlignment(FVector2D(0.f, 0.f));
			CanvasSlot->SetOffsets(FMargin(0.f));
			return true;
		}

		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
		{
			OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
			OverlaySlot->SetVerticalAlignment(VAlign_Fill);
			return true;
		}

		return false;
	}

	static bool SetHorizontalBoxFill(UWidget* Widget, const float FillValue)
	{
		if (!Widget || !Widget->Slot)
		{
			return false;
		}

		if (UHorizontalBoxSlot* HorizontalSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		{
			FSlateChildSize Size;
			Size.SizeRule = ESlateSizeRule::Fill;
			Size.Value = FillValue;
			HorizontalSlot->SetSize(Size);
			HorizontalSlot->SetHorizontalAlignment(HAlign_Fill);
			HorizontalSlot->SetVerticalAlignment(VAlign_Fill);
			return true;
		}

		return false;
	}

	static int32 StretchRootChildrenToFill(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree)
		{
			return 0;
		}

		UWidget* RootWidget = BP->WidgetTree->RootWidget;
		UPanelWidget* RootPanel = Cast<UPanelWidget>(RootWidget);
		if (!RootPanel)
		{
			return 0;
		}

		int32 Updated = 0;
		for (int32 Index = 0; Index < RootPanel->GetChildrenCount(); ++Index)
		{
			Updated += StretchToFill(RootPanel->GetChildAt(Index)) ? 1 : 0;
		}

		return Updated;
	}

	static UPanelWidget* GetOrCreateLeftMenuPanel(UWidgetBlueprint* BP)
	{
		if (!BP || !BP->WidgetTree)
		{
			return nullptr;
		}

		UWidget* LeftPanelWidget = BP->WidgetTree->FindWidget(FName(TEXT("Panel_LeftMenu")));
		UBorder* LeftBorder = Cast<UBorder>(LeftPanelWidget);
		if (!LeftBorder)
		{
			return Cast<UPanelWidget>(LeftPanelWidget);
		}

		UWidget* ExistingContent = LeftBorder->GetContent();
		if (UPanelWidget* PanelContent = Cast<UPanelWidget>(ExistingContent))
		{
			return PanelContent;
		}

		UVerticalBox* NewBox = BP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(TEXT("VB_LeftMenu")));
		if (!NewBox)
		{
			return nullptr;
		}

		LeftBorder->SetContent(NewBox);

		if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(NewBox->Slot))
		{
			BorderSlot->SetHorizontalAlignment(HAlign_Left);
			BorderSlot->SetVerticalAlignment(VAlign_Center);
			BorderSlot->SetPadding(FMargin(48.f, 24.f, 24.f, 24.f));
		}

		if (ExistingContent)
		{
			NewBox->AddChild(ExistingContent);
		}

		return NewBox;
	}

	static int32 MoveButtonsToPanel(UWidgetBlueprint* BP, UPanelWidget* TargetPanel)
	{
		if (!BP || !BP->WidgetTree || !TargetPanel)
		{
			return 0;
		}

		const FName ButtonNames[] = {
			FName(TEXT("Button_NewGame")),
			FName(TEXT("Button_LoadGame")),
			FName(TEXT("Button_Settings")),
			FName(TEXT("Button_Achievements")),
			FName(TEXT("Button_AccountStatus")),
			FName(TEXT("Button_LanguageIcon")),
			FName(TEXT("Button_QuitIcon"))
		};

		int32 Updated = 0;
		for (const FName& Name : ButtonNames)
		{
			UWidget* Button = BP->WidgetTree->FindWidget(Name);
			if (!Button)
			{
				continue;
			}

			if (UPanelWidget* Parent = Button->GetParent())
			{
				Parent->RemoveChild(Button);
			}

			TargetPanel->AddChild(Button);
			Button->SetVisibility(ESlateVisibility::Visible);
			Button->SetRenderOpacity(1.f);
			Button->SetIsEnabled(true);

			if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(Button->Slot))
			{
				VBoxSlot->SetHorizontalAlignment(HAlign_Center);
				VBoxSlot->SetPadding(FMargin(0.f, 6.f));
			}

			++Updated;
		}

		return Updated;
	}
}

UT66MainMenuCenterCommandlet::UT66MainMenuCenterCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66MainMenuCenterCommandlet::Main(const FString& Params)
{
	UObject* Loaded = UEditorAssetLibrary::LoadAsset(T66_MAINMENU_WBP_PATH);
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
	if (!BP || !BP->WidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66MainMenuCenter] Failed to load WBP_Screen_MainMenu at: %s"), T66_MAINMENU_WBP_PATH);
		return 1;
	}

	BP->Modify();

	int32 CenteredCount = 0;
	CenteredCount += StretchRootChildrenToFill(BP);
	CenteredCount += SetHorizontalBoxFill(BP->WidgetTree->FindWidget(FName(TEXT("Panel_LeftMenu"))), 1.f);
	CenteredCount += SetHorizontalBoxFill(BP->WidgetTree->FindWidget(FName(TEXT("Panel_RightLeaderboard"))), 1.f);
	CenteredCount += MoveButtonsToPanel(BP, GetOrCreateLeftMenuPanel(BP));
	CenteredCount += CenterWidgetSlot(BP->WidgetTree->FindWidget(FName(TEXT("Button_NewGame"))));
	CenteredCount += CenterWidgetSlot(BP->WidgetTree->FindWidget(FName(TEXT("Button_LoadGame"))));
	CenteredCount += CenterWidgetSlot(BP->WidgetTree->FindWidget(FName(TEXT("Button_Settings"))));
	CenteredCount += CenterWidgetSlot(BP->WidgetTree->FindWidget(FName(TEXT("Button_Achievements"))));
	CenteredCount += CenterWidgetSlot(BP->WidgetTree->FindWidget(FName(TEXT("Button_AccountStatus"))));
	CenteredCount += CenterWidgetSlot(BP->WidgetTree->FindWidget(FName(TEXT("Button_LanguageIcon"))));
	CenteredCount += CenterWidgetSlot(BP->WidgetTree->FindWidget(FName(TEXT("Button_QuitIcon"))));

	BP->MarkPackageDirty();
	const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(BP, /*bOnlyIfIsDirty=*/true);

	UE_LOG(LogTemp, Display, TEXT("[T66MainMenuCenter] Centered=%d | Saved=%s"), CenteredCount, bSaved ? TEXT("true") : TEXT("false"));
	return bSaved ? 0 : 1;
}
