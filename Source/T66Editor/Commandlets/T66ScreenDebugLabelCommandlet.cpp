#include "T66ScreenDebugLabelCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "EditorAssetLibrary.h"
#include "WidgetBlueprint.h"

static const FName DebugLabelName(TEXT("Text_DebugScreenName"));
static const TCHAR* ScreenPrefix = TEXT("WBP_Screen_");

namespace
{
	static void SetTextBlockDefaults(UTextBlock* Text, const FString& ScreenName)
	{
		Text->SetText(FText::FromString(FString::Printf(TEXT("Screen: %s"), *ScreenName)));
		Text->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.85f)));
		Text->SetJustification(ETextJustify::Center);
		Text->SetShadowOffset(FVector2D(1.f, 1.f));
		Text->SetShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.85f));
	}

	static void CenterLabelSlot(UWidget* Widget)
	{
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
			Offsets.Top = 16.f;
			Offsets.Right = 400.f;
			Offsets.Bottom = 40.f;
			CanvasSlot->SetOffsets(Offsets);
		}
		else if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
		{
			OverlaySlot->SetPadding(FMargin(0.f, 16.f, 0.f, 0.f));
			OverlaySlot->SetHorizontalAlignment(HAlign_Center);
			OverlaySlot->SetVerticalAlignment(VAlign_Top);
		}
	}

	static void EnsureWidgetGuid(UWidgetBlueprint* BP, UWidget* Widget)
	{
		if (!BP || !Widget)
		{
			return;
		}

		if (!BP->WidgetVariableNameToGuidMap.Contains(Widget->GetFName()))
		{
			BP->OnVariableAdded(Widget->GetFName());
		}
	}

	static bool AttachToCanvas(UWidgetBlueprint* BP, UCanvasPanel* Canvas, UTextBlock* Text)
	{
		if (!Canvas || !Text)
		{
			return false;
		}

		Canvas->AddChild(Text);
		CenterLabelSlot(Text);

		return true;
	}

	static bool AttachToOverlay(UOverlay* Overlay, UTextBlock* Text)
	{
		if (!Overlay || !Text)
		{
			return false;
		}

		Overlay->AddChild(Text);
		CenterLabelSlot(Text);

		return true;
	}
}

UT66ScreenDebugLabelCommandlet::UT66ScreenDebugLabelCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66ScreenDebugLabelCommandlet::Main(const FString& Params)
{
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.PackagePaths.Add(FName(TEXT("/Game")));
	Filter.ClassPaths.Add(UWidgetBlueprint::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;

	TArray<FAssetData> Assets;
	AssetRegistry.GetAssets(Filter, Assets);

	int32 UpdatedCount = 0;
	int32 SkippedCount = 0;

	for (const FAssetData& Asset : Assets)
	{
		const FString AssetName = Asset.AssetName.ToString();
		if (!AssetName.StartsWith(ScreenPrefix))
		{
			continue;
		}

		UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Asset.GetAsset());
		if (!BP || !BP->WidgetTree)
		{
			++SkippedCount;
			continue;
		}

		const FString ScreenName = AssetName.RightChop(FCString::Strlen(ScreenPrefix));
		UWidget* Existing = BP->WidgetTree->FindWidget(DebugLabelName);
		UTextBlock* Label = Existing ? Cast<UTextBlock>(Existing) : nullptr;

		BP->Modify();

		if (!Label)
		{
			Label = BP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), DebugLabelName);
			if (!Label)
			{
				++SkippedCount;
				continue;
			}

			UWidget* RootWidget = BP->WidgetTree->RootWidget;
			if (UCanvasPanel* Canvas = Cast<UCanvasPanel>(RootWidget))
			{
				AttachToCanvas(BP, Canvas, Label);
			}
			else if (UOverlay* Overlay = Cast<UOverlay>(RootWidget))
			{
				AttachToOverlay(Overlay, Label);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[T66ScreenDebugLabel] Skipping %s (unsupported root)"), *AssetName);
				++SkippedCount;
				continue;
			}
		}

		SetTextBlockDefaults(Label, ScreenName);
		CenterLabelSlot(Label);
		EnsureWidgetGuid(BP, Label);

		BP->MarkPackageDirty();
		const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(BP, /*bOnlyIfIsDirty=*/true);
		if (bSaved)
		{
			++UpdatedCount;
		}
	}

	UE_LOG(LogTemp, Display, TEXT("[T66ScreenDebugLabel] Updated=%d | Skipped=%d"), UpdatedCount, SkippedCount);
	return 0;
}
