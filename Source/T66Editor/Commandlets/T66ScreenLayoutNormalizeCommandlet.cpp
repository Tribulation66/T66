#include "T66ScreenLayoutNormalizeCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ContentWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "EditorAssetLibrary.h"
#include "WidgetBlueprint.h"

static const TCHAR* ScreenPrefix = TEXT("WBP_Screen_");
static const TCHAR* ActionButtonPath = TEXT("/Game/Tribulation66/Content/UI/Components/Button/WBP_Comp_Button_Action.WBP_Comp_Button_Action");
static const TCHAR* IconButtonPath = TEXT("/Game/Tribulation66/Content/UI/Components/Button/WBP_Comp_Button_IconOnly.WBP_Comp_Button_IconOnly");

namespace
{
	static bool IsPanelWidget(UWidget* Widget)
	{
		return Widget && (Cast<UPanelWidget>(Widget) != nullptr);
	}

	static bool NormalizeRootChildSlot(UWidgetBlueprint* BP, UWidget* Widget)
	{
		if (!BP || !BP->WidgetTree || !Widget)
		{
			return false;
		}

		if (Widget->GetParent() != BP->WidgetTree->RootWidget)
		{
			return false;
		}

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			CanvasSlot->SetOffsets(FMargin(0.f));
			CanvasSlot->SetAlignment(FVector2D(0.f, 0.f));
			return true;
		}

		return false;
	}

	static bool NormalizeHorizontalFill(UWidget* Widget)
	{
		if (!Widget || !Widget->Slot)
		{
			return false;
		}

		if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		{
			if (IsPanelWidget(Widget))
			{
				FSlateChildSize Size;
				Size.SizeRule = ESlateSizeRule::Fill;
				Size.Value = 1.f;
				Slot->SetSize(Size);
				Slot->SetHorizontalAlignment(HAlign_Fill);
				Slot->SetVerticalAlignment(VAlign_Fill);
				return true;
			}
		}

		return false;
	}

	static bool ClearSizeBoxOverrides(USizeBox* SizeBox)
	{
		if (!SizeBox)
		{
			return false;
		}

		bool bChanged = false;
		if (SizeBox->IsWidthOverride())
		{
			SizeBox->ClearWidthOverride();
			bChanged = true;
		}
		if (SizeBox->IsHeightOverride())
		{
			SizeBox->ClearHeightOverride();
			bChanged = true;
		}
		if (SizeBox->IsMinDesiredWidthOverride())
		{
			SizeBox->ClearMinDesiredWidth();
			bChanged = true;
		}
		if (SizeBox->IsMinDesiredHeightOverride())
		{
			SizeBox->ClearMinDesiredHeight();
			bChanged = true;
		}
		if (SizeBox->IsMaxDesiredWidthOverride())
		{
			SizeBox->ClearMaxDesiredWidth();
			bChanged = true;
		}
		if (SizeBox->IsMaxDesiredHeightOverride())
		{
			SizeBox->ClearMaxDesiredHeight();
			bChanged = true;
		}
		if (SizeBox->IsMinAspectRatioOverride())
		{
			SizeBox->ClearMinAspectRatio();
			bChanged = true;
		}
		if (SizeBox->IsMaxAspectRatioOverride())
		{
			SizeBox->ClearMaxAspectRatio();
			bChanged = true;
		}

		return bChanged;
	}

	static bool ForceVisibleIfNamed(UWidgetBlueprint* BP, UWidget* Widget)
	{
		if (!BP || !Widget)
		{
			return false;
		}

		const FString Name = Widget->GetName();
		if (Name.StartsWith(TEXT("Button_")) || Name.StartsWith(TEXT("Panel_")) || Name.StartsWith(TEXT("VB_")))
		{
			Widget->Modify();
			if (Widget->VisibilityDelegate.IsBound())
			{
				Widget->VisibilityDelegate.Unbind();
			}
			BP->Bindings.RemoveAll([Widget](const FDelegateEditorBinding& Binding)
			{
				return Binding.ObjectName == Widget->GetName()
					&& Binding.PropertyName == FName(TEXT("Visibility"));
			});

			Widget->SetVisibility(ESlateVisibility::Visible);
			Widget->SetRenderOpacity(1.f);
			Widget->SetIsEnabled(true);
			Widget->PostEditChange();

			if (FProperty* VisibilityProp = Widget->GetClass()->FindPropertyByName(FName(TEXT("Visibility"))))
			{
				if (FEnumProperty* EnumProp = CastField<FEnumProperty>(VisibilityProp))
				{
					uint8* ValuePtr = EnumProp->ContainerPtrToValuePtr<uint8>(Widget);
					if (ValuePtr)
					{
						*ValuePtr = static_cast<uint8>(ESlateVisibility::Visible);
					}
				}
				else if (FByteProperty* ByteProp = CastField<FByteProperty>(VisibilityProp))
				{
					uint8* ValuePtr = ByteProp->ContainerPtrToValuePtr<uint8>(Widget);
					if (ValuePtr)
					{
						*ValuePtr = static_cast<uint8>(ESlateVisibility::Visible);
					}
				}
			}

			return true;
		}

		return false;
	}

	static int32 NormalizeWidgetTree(UWidgetBlueprint* BP, UWidget* Widget)
	{
		if (!Widget)
		{
			return 0;
		}

		int32 Updated = 0;
		Updated += NormalizeRootChildSlot(BP, Widget) ? 1 : 0;
		Updated += NormalizeHorizontalFill(Widget) ? 1 : 0;
		Updated += ForceVisibleIfNamed(BP, Widget) ? 1 : 0;

		if (USizeBox* SizeBox = Cast<USizeBox>(Widget))
		{
			Updated += ClearSizeBoxOverrides(SizeBox) ? 1 : 0;
		}

		if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
		{
			const int32 ChildCount = Panel->GetChildrenCount();
			for (int32 Index = 0; Index < ChildCount; ++Index)
			{
				Updated += NormalizeWidgetTree(BP, Panel->GetChildAt(Index));
			}
		}
		else if (UContentWidget* Content = Cast<UContentWidget>(Widget))
		{
			Updated += NormalizeWidgetTree(BP, Content->GetContent());
		}

		return Updated;
	}

	static bool NormalizeButtonComponent(const TCHAR* AssetPath)
	{
		UObject* Loaded = UEditorAssetLibrary::LoadAsset(AssetPath);
		UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
		if (!BP || !BP->WidgetTree || !BP->WidgetTree->RootWidget)
		{
			return false;
		}

		BP->Modify();
		NormalizeWidgetTree(BP, BP->WidgetTree->RootWidget);
		BP->MarkPackageDirty();
		return UEditorAssetLibrary::SaveLoadedAsset(BP, /*bOnlyIfIsDirty=*/true);
	}
}

UT66ScreenLayoutNormalizeCommandlet::UT66ScreenLayoutNormalizeCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66ScreenLayoutNormalizeCommandlet::Main(const FString& Params)
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

	NormalizeButtonComponent(ActionButtonPath);
	NormalizeButtonComponent(IconButtonPath);

	for (const FAssetData& Asset : Assets)
	{
		const FString AssetName = Asset.AssetName.ToString();
		if (!AssetName.StartsWith(ScreenPrefix))
		{
			continue;
		}

		UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Asset.GetAsset());
		if (!BP || !BP->WidgetTree || !BP->WidgetTree->RootWidget)
		{
			++SkippedCount;
			continue;
		}

		BP->Modify();
		BP->WidgetTree->Modify();
		const int32 Updated = NormalizeWidgetTree(BP, BP->WidgetTree->RootWidget);
		if (Updated > 0)
		{
			BP->MarkPackageDirty();
			if (UEditorAssetLibrary::SaveLoadedAsset(BP, /*bOnlyIfIsDirty=*/true))
			{
				++UpdatedCount;
			}
		}
	}

	UE_LOG(LogTemp, Display, TEXT("[T66ScreenLayoutNormalize] Updated=%d | Skipped=%d"), UpdatedCount, SkippedCount);
	return 0;
}
