#include "T66MainMenuDumpCommandlet.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ContentWidget.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/PanelWidget.h"
#include "Components/ScaleBox.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBoxSlot.h"
#include "EditorAssetLibrary.h"
#include "WidgetBlueprint.h"

static const TCHAR* T66_MAINMENU_WBP_PATH = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_MainMenu.WBP_Screen_MainMenu");

namespace
{
	static void AppendSlotInfo(FString& Out, UWidget* Widget)
	{
		if (!Widget || !Widget->Slot)
		{
			return;
		}

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			const FAnchors Anchors = CanvasSlot->GetAnchors();
			const FMargin Offsets = CanvasSlot->GetOffsets();
			const FVector2D Alignment = CanvasSlot->GetAlignment();
			Out += FString::Printf(TEXT(" [Canvas anchors=(%.2f,%.2f)-(%.2f,%.2f) offsets=(%.1f,%.1f,%.1f,%.1f) align=(%.2f,%.2f)]"),
				Anchors.Minimum.X, Anchors.Minimum.Y, Anchors.Maximum.X, Anchors.Maximum.Y,
				Offsets.Left, Offsets.Top, Offsets.Right, Offsets.Bottom,
				Alignment.X, Alignment.Y);
		}
		else if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
		{
			Out += FString::Printf(TEXT(" [Overlay h=%d v=%d padding=(%.1f,%.1f,%.1f,%.1f)]"),
				static_cast<int32>(OverlaySlot->GetHorizontalAlignment()),
				static_cast<int32>(OverlaySlot->GetVerticalAlignment()),
				OverlaySlot->GetPadding().Left, OverlaySlot->GetPadding().Top,
				OverlaySlot->GetPadding().Right, OverlaySlot->GetPadding().Bottom);
		}
		else if (UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		{
			Out += FString::Printf(TEXT(" [VBox h=%d v=%d padding=(%.1f,%.1f,%.1f,%.1f)]"),
				static_cast<int32>(VerticalSlot->GetHorizontalAlignment()),
				static_cast<int32>(VerticalSlot->GetVerticalAlignment()),
				VerticalSlot->GetPadding().Left, VerticalSlot->GetPadding().Top,
				VerticalSlot->GetPadding().Right, VerticalSlot->GetPadding().Bottom);
		}
		else if (UHorizontalBoxSlot* HorizontalSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		{
			const FSlateChildSize Size = HorizontalSlot->GetSize();
			Out += FString::Printf(TEXT(" [HBox size=%s fill=%.2f h=%d v=%d padding=(%.1f,%.1f,%.1f,%.1f)]"),
				Size.SizeRule == ESlateSizeRule::Fill ? TEXT("Fill") : TEXT("Auto"),
				Size.Value,
				static_cast<int32>(HorizontalSlot->GetHorizontalAlignment()),
				static_cast<int32>(HorizontalSlot->GetVerticalAlignment()),
				HorizontalSlot->GetPadding().Left, HorizontalSlot->GetPadding().Top,
				HorizontalSlot->GetPadding().Right, HorizontalSlot->GetPadding().Bottom);
		}
	}

	static void AppendWidgetInfo(FString& Out, UWidget* Widget)
	{
		if (!Widget)
		{
			return;
		}

		Out += FString::Printf(TEXT("%s (%s)"),
			*Widget->GetName(),
			*Widget->GetClass()->GetName());

		if (const USizeBox* SizeBox = Cast<USizeBox>(Widget))
		{
			Out += FString::Printf(TEXT(" [SizeBox override W=%s H=%s]"),
				SizeBox->IsWidthOverride() ? TEXT("true") : TEXT("false"),
				SizeBox->IsHeightOverride() ? TEXT("true") : TEXT("false"));

			if (SizeBox->IsWidthOverride())
			{
				Out += FString::Printf(TEXT(" [W=%.1f]"), SizeBox->GetWidthOverride());
			}
			if (SizeBox->IsHeightOverride())
			{
				Out += FString::Printf(TEXT(" [H=%.1f]"), SizeBox->GetHeightOverride());
			}
		}
		else if (const UScaleBox* ScaleBox = Cast<UScaleBox>(Widget))
		{
			Out += FString::Printf(TEXT(" [ScaleBox stretch=%d dir=%d]"),
				static_cast<int32>(ScaleBox->GetStretch()),
				static_cast<int32>(ScaleBox->GetStretchDirection()));
		}
	}

	static void DumpWidget(UWidget* Widget, int32 Depth)
	{
		if (!Widget)
		{
			return;
		}

		FString Line;
		Line.AppendChars(TEXT("  "), Depth);
		AppendWidgetInfo(Line, Widget);
		AppendSlotInfo(Line, Widget);
		UE_LOG(LogTemp, Display, TEXT("%s"), *Line);

		if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
		{
			const int32 ChildCount = Panel->GetChildrenCount();
			for (int32 Index = 0; Index < ChildCount; ++Index)
			{
				DumpWidget(Panel->GetChildAt(Index), Depth + 1);
			}
		}
		else if (UContentWidget* Content = Cast<UContentWidget>(Widget))
		{
			DumpWidget(Content->GetContent(), Depth + 1);
		}
	}

	static void DumpWidgetChain(UWidget* Widget, const FString& Label)
	{
		if (!Widget)
		{
			UE_LOG(LogTemp, Display, TEXT("[T66MainMenuDump] %s: <missing>"), *Label);
			return;
		}

		FString Chain = FString::Printf(TEXT("%s: %s (%s) vis=%d"),
			*Label,
			*Widget->GetName(),
			*Widget->GetClass()->GetName(),
			static_cast<int32>(Widget->GetVisibility()));
		Chain += FString::Printf(TEXT(" opacity=%.2f enabled=%s"),
			Widget->GetRenderOpacity(),
			Widget->GetIsEnabled() ? TEXT("true") : TEXT("false"));

		UWidget* Current = Widget;
		int32 Depth = 0;
		while (Current && Depth < 10)
		{
			UWidget* Parent = Current->GetParent();
			if (!Parent)
			{
				break;
			}

			Chain += FString::Printf(TEXT(" <- %s(%s)"),
				*Parent->GetName(),
				*Parent->GetClass()->GetName());
			Current = Parent;
			++Depth;
		}

		UE_LOG(LogTemp, Display, TEXT("[T66MainMenuDump] %s"), *Chain);
	}
}

UT66MainMenuDumpCommandlet::UT66MainMenuDumpCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66MainMenuDumpCommandlet::Main(const FString& Params)
{
	UObject* Loaded = UEditorAssetLibrary::LoadAsset(T66_MAINMENU_WBP_PATH);
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
	if (!BP || !BP->WidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66MainMenuDump] Failed to load WBP_Screen_MainMenu at: %s"), T66_MAINMENU_WBP_PATH);
		return 1;
	}

	UE_LOG(LogTemp, Display, TEXT("[T66MainMenuDump] Root:"));
	DumpWidget(BP->WidgetTree->RootWidget, 0);

	DumpWidgetChain(BP->WidgetTree->FindWidget(FName(TEXT("Button_NewGame"))), TEXT("Button_NewGame"));
	DumpWidgetChain(BP->WidgetTree->FindWidget(FName(TEXT("Button_LoadGame"))), TEXT("Button_LoadGame"));
	DumpWidgetChain(BP->WidgetTree->FindWidget(FName(TEXT("Button_Settings"))), TEXT("Button_Settings"));
	DumpWidgetChain(BP->WidgetTree->FindWidget(FName(TEXT("Button_Achievements"))), TEXT("Button_Achievements"));
	DumpWidgetChain(BP->WidgetTree->FindWidget(FName(TEXT("Button_AccountStatus"))), TEXT("Button_AccountStatus"));
	DumpWidgetChain(BP->WidgetTree->FindWidget(FName(TEXT("Button_LanguageIcon"))), TEXT("Button_LanguageIcon"));
	DumpWidgetChain(BP->WidgetTree->FindWidget(FName(TEXT("Button_QuitIcon"))), TEXT("Button_QuitIcon"));
	DumpWidgetChain(BP->WidgetTree->FindWidget(FName(TEXT("Panel_LeftMenu"))), TEXT("Panel_LeftMenu"));
	DumpWidgetChain(BP->WidgetTree->FindWidget(FName(TEXT("VB_LeftMenu"))), TEXT("VB_LeftMenu"));
	return 0;
}
