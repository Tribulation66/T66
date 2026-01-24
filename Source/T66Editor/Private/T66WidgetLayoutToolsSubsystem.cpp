#include "T66WidgetLayoutToolsSubsystem.h"

#include "T66WidgetLayoutRecipes.h"

#include "EditorAssetLibrary.h"
#include "BlueprintEditorLibrary.h"
#include "EdGraphSchema_K2.h"

#include "GameplayTagContainer.h"

// Widget BP + WidgetTree editing
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"

// UMG widgets to construct (for the shared button component repair)
#include "Components/Button.h"
#include "Components/TextBlock.h"

// Content Browser selection
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Modules/ModuleManager.h"

static const TCHAR* T66_ACTION_BUTTON_BP_PATH =
TEXT("/Game/Tribulation66/Content/UI/Components/Button/WBP_Comp_Button_Action.WBP_Comp_Button_Action");

static const FName VAR_ControlID(TEXT("ControlID"));
static const FName VAR_ActionTag(TEXT("ActionTag"));
static const FName VAR_RouteTag(TEXT("RouteTag"));
static const FName VAR_LabelText(TEXT("LabelText"));

static const FName WIDGET_ButtonRoot(TEXT("Button_Root"));
static const FName WIDGET_TextLabel(TEXT("Text_Label"));

namespace T66WidgetLayoutTools_Internal
{
	static bool HasMemberVariable(UBlueprint* BP, const FName VarName)
	{
		if (!BP)
		{
			return false;
		}

		for (const FBPVariableDescription& Desc : BP->NewVariables)
		{
			if (Desc.VarName == VarName)
			{
				return true;
			}
		}
		return false;
	}

	static void EnsureVariable(UBlueprint* BP, const FName VarName, const FEdGraphPinType& PinType)
	{
		if (!BP)
		{
			return;
		}

		if (HasMemberVariable(BP, VarName))
		{
			return;
		}

		const bool bAdded = UBlueprintEditorLibrary::AddMemberVariable(BP, VarName, PinType);
		if (!bAdded)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66WidgetLayoutTools] Failed to add variable '%s'"), *VarName.ToString());
			return;
		}

		UBlueprintEditorLibrary::SetBlueprintVariableInstanceEditable(BP, VarName, true);
		UBlueprintEditorLibrary::SetBlueprintVariableExposeOnSpawn(BP, VarName, true);

		UE_LOG(LogTemp, Display, TEXT("[T66WidgetLayoutTools] Added variable '%s'"), *VarName.ToString());
	}

	static void EnsureMinimalWidgetTree(UWidgetBlueprint* WidgetBP)
	{
		if (!WidgetBP || !WidgetBP->WidgetTree)
		{
			return;
		}

		// ✅ Add/Repair only: if root exists, do NOT touch layout
		if (WidgetBP->WidgetTree->RootWidget)
		{
			return;
		}

		// Build: Button_Root -> Text_Label
		UButton* ButtonRoot = WidgetBP->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), WIDGET_ButtonRoot);
		if (!ButtonRoot)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66WidgetLayoutTools] Failed to construct Button_Root"));
			return;
		}

		UTextBlock* TextLabel = WidgetBP->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), WIDGET_TextLabel);
		if (!TextLabel)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66WidgetLayoutTools] Failed to construct Text_Label"));
			return;
		}

		ButtonRoot->SetContent(TextLabel);
		WidgetBP->WidgetTree->RootWidget = ButtonRoot;

		UE_LOG(LogTemp, Display, TEXT("[T66WidgetLayoutTools] Created minimal widget tree for WBP_Comp_Button_Action"));
	}
}

void UT66WidgetLayoutToolsSubsystem::CreateOrRepairUIButtonComponents()
{
	UObject* LoadedObj = UEditorAssetLibrary::LoadAsset(T66_ACTION_BUTTON_BP_PATH);
	if (!LoadedObj)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66WidgetLayoutTools] Failed to load asset: %s"), T66_ACTION_BUTTON_BP_PATH);
		return;
	}

	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(LoadedObj);
	if (!WidgetBP)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66WidgetLayoutTools] Asset is not a UWidgetBlueprint: %s"), T66_ACTION_BUTTON_BP_PATH);
		return;
	}

	WidgetBP->Modify();

	// GameplayTag pin type
	FEdGraphPinType GameplayTagType;
	GameplayTagType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	GameplayTagType.PinSubCategoryObject = FGameplayTag::StaticStruct();

	// FText pin type
	FEdGraphPinType TextType;
	TextType.PinCategory = UEdGraphSchema_K2::PC_Text;

	// ✅ Ensure exposed variables exist (repair/add only)
	T66WidgetLayoutTools_Internal::EnsureVariable(WidgetBP, VAR_ControlID, GameplayTagType);
	T66WidgetLayoutTools_Internal::EnsureVariable(WidgetBP, VAR_ActionTag, GameplayTagType);
	T66WidgetLayoutTools_Internal::EnsureVariable(WidgetBP, VAR_RouteTag, GameplayTagType);
	T66WidgetLayoutTools_Internal::EnsureVariable(WidgetBP, VAR_LabelText, TextType);

	// ✅ Ensure minimal widget tree exists (repair/add only)
	T66WidgetLayoutTools_Internal::EnsureMinimalWidgetTree(WidgetBP);

	// Compile + save
	UBlueprintEditorLibrary::CompileBlueprint(WidgetBP);

	WidgetBP->MarkPackageDirty();
	const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(WidgetBP, /*bOnlyIfIsDirty=*/true);

	UE_LOG(LogTemp, Display, TEXT("[T66WidgetLayoutTools] CreateOrRepairUIButtonComponents complete | Saved=%s"),
		bSaved ? TEXT("true") : TEXT("false"));
}

void UT66WidgetLayoutToolsSubsystem::BuildMinimumLayoutsForSelectedWidgetBlueprints()
{
	// ------------------------------------------------------------
	// Selection-based stamping (repair/add-only)
	// Uses explicit per-widget recipes only (no generic fallback)
	// ------------------------------------------------------------

	FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	TArray<FAssetData> SelectedAssets;
	CBModule.Get().GetSelectedAssets(SelectedAssets);

	if (SelectedAssets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutTools] No assets selected in Content Browser."));
		return;
	}

	int32 NumConsidered = 0;
	int32 NumStamped = 0;
	int32 NumSaved = 0;

	for (const FAssetData& AssetData : SelectedAssets)
	{
		UObject* Obj = AssetData.GetAsset();
		if (!Obj)
		{
			continue;
		}

		UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(Obj);
		if (!WidgetBP)
		{
			UE_LOG(LogTemp, Display, TEXT("[T66WidgetLayoutTools] Skipping non-WidgetBlueprint: %s"), *Obj->GetName());
			continue;
		}

		NumConsidered++;

		// ✅ Apply an explicit recipe based on widget name
		const bool bApplied = T66WidgetLayoutRecipes::TryApplyRecipe(WidgetBP);
		if (!bApplied)
		{
			// Recipe library logs why (no recipe or safety skip)
			continue;
		}

		NumStamped++;

		// Compile + save
		UBlueprintEditorLibrary::CompileBlueprint(WidgetBP);

		WidgetBP->MarkPackageDirty();
		const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(WidgetBP, /*bOnlyIfIsDirty=*/true);
		if (bSaved)
		{
			NumSaved++;
		}

		UE_LOG(LogTemp, Display, TEXT("[T66WidgetLayoutTools] Stamped: %s | Saved=%s"),
			*WidgetBP->GetName(),
			bSaved ? TEXT("true") : TEXT("false"));
	}

	UE_LOG(LogTemp, Display, TEXT("[T66WidgetLayoutTools] BuildMinimumLayouts complete. Considered=%d | Stamped=%d | Saved=%d"),
		NumConsidered, NumStamped, NumSaved);
}
