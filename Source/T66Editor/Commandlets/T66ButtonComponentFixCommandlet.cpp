#include "T66ButtonComponentFixCommandlet.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "EditorAssetLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "WidgetBlueprint.h"

static const TCHAR* T66_ACTION_BUTTON_PATH = TEXT("/Game/Tribulation66/Content/UI/Components/Button/WBP_Comp_Button_Action.WBP_Comp_Button_Action");
static const TCHAR* T66_ICON_BUTTON_PATH = TEXT("/Game/Tribulation66/Content/UI/Components/Button/WBP_Comp_Button_IconOnly.WBP_Comp_Button_IconOnly");

namespace
{
	static void EnsureWidgetGuid(UWidgetBlueprint* BP, UWidget* Widget)
	{
		if (BP && Widget && !BP->WidgetVariableNameToGuidMap.Contains(Widget->GetFName()))
		{
			BP->OnVariableAdded(Widget->GetFName());
		}
	}

	static void DumpWidgetTree(UWidget* Widget, int32 Indent = 0)
	{
		if (!Widget)
		{
			return;
		}

		FString IndentStr;
		for (int32 i = 0; i < Indent; ++i)
		{
			IndentStr += TEXT("  ");
		}

		UE_LOG(LogTemp, Display, TEXT("%s%s (%s) vis=%d opacity=%.2f"),
			*IndentStr,
			*Widget->GetName(),
			*Widget->GetClass()->GetName(),
			static_cast<int32>(Widget->GetVisibility()),
			Widget->GetRenderOpacity());

		if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
		{
			for (int32 i = 0; i < Panel->GetChildrenCount(); ++i)
			{
				DumpWidgetTree(Panel->GetChildAt(i), Indent + 1);
			}
		}
		else if (UContentWidget* Content = Cast<UContentWidget>(Widget))
		{
			DumpWidgetTree(Content->GetContent(), Indent + 1);
		}
	}

	static bool FixButtonWidget(const TCHAR* AssetPath, bool bIsIconButton)
	{
		UObject* Loaded = UEditorAssetLibrary::LoadAsset(AssetPath);
		UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
		if (!BP)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66ButtonComponentFix] Failed to load: %s"), AssetPath);
			return false;
		}

		UE_LOG(LogTemp, Display, TEXT("[T66ButtonComponentFix] Processing: %s"), AssetPath);

		// Dump existing structure
		if (BP->WidgetTree && BP->WidgetTree->RootWidget)
		{
			UE_LOG(LogTemp, Display, TEXT("[T66ButtonComponentFix] Existing structure:"));
			DumpWidgetTree(BP->WidgetTree->RootWidget);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("[T66ButtonComponentFix] No existing widget tree"));
		}

		BP->Modify();

		// Clear old GUID map to prevent stale references
		BP->WidgetVariableNameToGuidMap.Empty();

		// Create new widget tree
		UWidgetTree* NewTree = NewObject<UWidgetTree>(BP, UWidgetTree::StaticClass(), NAME_None, RF_Transactional);
		if (!NewTree)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66ButtonComponentFix] Failed to create widget tree"));
			return false;
		}

		BP->WidgetTree = NewTree;

		// Create structure: SizeBox > Button > Overlay > Border + TextBlock
		USizeBox* SizeBox = BP->WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(TEXT("SizeBox_Root")));
		BP->WidgetTree->RootWidget = SizeBox;
		SizeBox->SetMinDesiredWidth(200.f);
		SizeBox->SetMinDesiredHeight(50.f);

		UButton* Button = BP->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(TEXT("Button_Root")));
		SizeBox->SetContent(Button);
		Button->SetVisibility(ESlateVisibility::Visible);

		// Set button style to be visible
		FButtonStyle ButtonStyle = Button->GetStyle();
		
		// Normal state - dark blue
		ButtonStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.2f, 1.f));
		ButtonStyle.Normal.DrawAs = ESlateBrushDrawType::Box;
		
		// Hovered state - lighter blue
		ButtonStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.2f, 0.4f, 1.f));
		ButtonStyle.Hovered.DrawAs = ESlateBrushDrawType::Box;
		
		// Pressed state - even lighter
		ButtonStyle.Pressed.TintColor = FSlateColor(FLinearColor(0.3f, 0.3f, 0.5f, 1.f));
		ButtonStyle.Pressed.DrawAs = ESlateBrushDrawType::Box;
		
		Button->SetStyle(ButtonStyle);

		UOverlay* Overlay = BP->WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName(TEXT("Overlay_Content")));
		Button->SetContent(Overlay);

		// Add background border for more visibility
		UBorder* Background = BP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(TEXT("Border_Background")));
		Background->SetBrushColor(FLinearColor(0.15f, 0.15f, 0.25f, 1.f));
		Background->SetPadding(FMargin(16.f, 8.f));
		Overlay->AddChild(Background);

		if (UOverlaySlot* BgSlot = Cast<UOverlaySlot>(Background->Slot))
		{
			BgSlot->SetHorizontalAlignment(HAlign_Fill);
			BgSlot->SetVerticalAlignment(VAlign_Fill);
		}

		if (!bIsIconButton)
		{
			// Add text for action buttons
			UTextBlock* Text = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Label")));
			Text->SetText(FText::FromString(TEXT("Button")));
			Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			
			FSlateFontInfo Font = Text->GetFont();
			Font.Size = 18;
			Text->SetFont(Font);
			
			Text->SetJustification(ETextJustify::Center);
			Text->SetVisibility(ESlateVisibility::Visible);

			Overlay->AddChild(Text);

			if (UOverlaySlot* TextSlot = Cast<UOverlaySlot>(Text->Slot))
			{
				TextSlot->SetHorizontalAlignment(HAlign_Center);
				TextSlot->SetVerticalAlignment(VAlign_Center);
			}

			EnsureWidgetGuid(BP, Text);
		}
		else
		{
			// For icon buttons, just make it smaller with an icon placeholder
			SizeBox->SetMinDesiredWidth(50.f);
			SizeBox->SetMinDesiredHeight(50.f);

			UTextBlock* IconText = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("Text_Icon")));
			IconText->SetText(FText::FromString(TEXT("⚙")));
			IconText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			
			FSlateFontInfo Font = IconText->GetFont();
			Font.Size = 24;
			IconText->SetFont(Font);
			
			IconText->SetJustification(ETextJustify::Center);

			Overlay->AddChild(IconText);

			if (UOverlaySlot* IconSlot = Cast<UOverlaySlot>(IconText->Slot))
			{
				IconSlot->SetHorizontalAlignment(HAlign_Center);
				IconSlot->SetVerticalAlignment(VAlign_Center);
			}

			EnsureWidgetGuid(BP, IconText);
		}

		EnsureWidgetGuid(BP, SizeBox);
		EnsureWidgetGuid(BP, Button);
		EnsureWidgetGuid(BP, Overlay);
		EnsureWidgetGuid(BP, Background);

		BP->MarkPackageDirty();
		const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(BP, /*bOnlyIfIsDirty=*/true);

		UE_LOG(LogTemp, Display, TEXT("[T66ButtonComponentFix] Saved=%s"), bSaved ? TEXT("true") : TEXT("false"));

		// Dump new structure
		if (BP->WidgetTree && BP->WidgetTree->RootWidget)
		{
			UE_LOG(LogTemp, Display, TEXT("[T66ButtonComponentFix] New structure:"));
			DumpWidgetTree(BP->WidgetTree->RootWidget);
		}

		return bSaved;
	}
}

UT66ButtonComponentFixCommandlet::UT66ButtonComponentFixCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66ButtonComponentFixCommandlet::Main(const FString& Params)
{
	bool bSuccess = true;

	bSuccess &= FixButtonWidget(T66_ACTION_BUTTON_PATH, false);
	bSuccess &= FixButtonWidget(T66_ICON_BUTTON_PATH, true);

	return bSuccess ? 0 : 1;
}
