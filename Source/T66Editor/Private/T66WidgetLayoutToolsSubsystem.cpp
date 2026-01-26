#include "T66WidgetLayoutToolsSubsystem.h"

#include "T66WidgetLayoutRecipes.h"

#include "EditorAssetLibrary.h"
#include "BlueprintEditorLibrary.h"
#include "EdGraphSchema_K2.h"

#include "Kismet2/BlueprintEditorUtils.h"

#include "GameplayTagContainer.h"

// Widget BP + WidgetTree editing
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"

// UMG widgets to construct (legacy targeted repair helper)
#include "Components/Button.h"
#include "Components/TextBlock.h"

// Content Browser selection
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Modules/ModuleManager.h"

// Selected asset type
#include "AssetRegistry/AssetData.h"
#include "UObject/UnrealType.h"

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
	/**
	 * FORCE OVERWRITE helper: replace the WidgetTree instance with a fresh one.
	 *
	 * Why this exists:
	 * - Simply removing children from the root can leave "orphan" widgets still owned by the WidgetTree.
	 * - The widget compiler may still consider those widgets during compilation/validation.
	 * - A fresh WidgetTree guarantees the recipe rebuild starts from a truly empty state.
	 */
	static void ForceReplaceWidgetTree(UWidgetBlueprint* WidgetBP)
	{
		if (!WidgetBP)
		{
			return;
		}

		WidgetBP->Modify();

		UWidgetTree* NewTree = NewObject<UWidgetTree>(WidgetBP, UWidgetTree::StaticClass(), NAME_None, RF_Transactional);
		if (!NewTree)
		{
			UE_LOG(LogTemp, Error, TEXT("[T66WidgetLayoutTools] ForceReplaceWidgetTree failed for %s"), *GetNameSafe(WidgetBP));
			return;
		}

		NewTree->SetFlags(RF_Transactional);
		WidgetBP->WidgetTree = NewTree;
	}

	/**
	 * HARD REPAIR: rebuild the entire GUID map to match the current WidgetTree variable widgets.
	 *
	 * Why this exists:
	 * - When we programmatically create / delete / rename Designer widgets, UE sometimes fails to update
	 *   WidgetVariableNameToGuidMap.
	 * - On compile, UE asserts if a variable widget exists without a GUID, or if the map contains stale keys.
	 *
	 * This function wipes and reconstructs the map (while preserving existing GUIDs when possible).
	 */
	static bool HardRebuildWidgetVariableNameGuidMap(UWidgetBlueprint* WidgetBP, const bool bLogChanges)
	{
		if (!WidgetBP || !WidgetBP->WidgetTree)
		{
			return false;
		}

		// NOTE:
		// UE's widget blueprint compiler expects *every* variable widget name to exist in WidgetVariableNameToGuidMap
		// BEFORE compiling, otherwise it triggers an ensure (WidgetBlueprintCompiler.cpp line ~794).
		//
		// We previously attempted to manipulate the map via FScriptMapHelper, but that approach can be brittle
		// (and in practice didn't reliably update the underlying TMap in UE 5.7).
		//
		// This version uses a typed TMap pointer via reflection, which is dramatically more reliable.
		FMapProperty* GuidMapProp = FindFProperty<FMapProperty>(WidgetBP->GetClass(), TEXT("WidgetVariableNameToGuidMap"));
		if (!GuidMapProp)
		{
			if (bLogChanges)
			{
				UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutTools] Could not find WidgetVariableNameToGuidMap on %s"), *GetNameSafe(WidgetBP));
			}
			return false;
		}

		TMap<FName, FGuid>* GuidMap = GuidMapProp->ContainerPtrToValuePtr<TMap<FName, FGuid>>(WidgetBP);
		if (!GuidMap)
		{
			if (bLogChanges)
			{
				UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutTools] Failed to access WidgetVariableNameToGuidMap memory on %s"), *GetNameSafe(WidgetBP));
			}
			return false;
		}

		// Snapshot existing GUIDs so stable widgets keep their GUIDs.
		const TMap<FName, FGuid> OldMap = *GuidMap;

		// Collect the current set of Designer widgets (ALL widgets, not just bIsVariable).
		// 
		// Why:
		// UE 5.7's WidgetBlueprintCompiler expects every Designer widget name to have a GUID entry.
		// If we only track bIsVariable widgets, compile will ensure/crash for newly-added non-variable widgets.
		TSet<FName> WidgetNames;
		{
			if (UWidget* RootWidget = WidgetBP->WidgetTree->RootWidget)
			{
				WidgetNames.Add(RootWidget->GetFName());
			}

			TArray<UWidget*> AllWidgets;
			WidgetBP->WidgetTree->GetAllWidgets(AllWidgets);
			for (UWidget* Widget : AllWidgets)
			{
				if (Widget)
				{
					WidgetNames.Add(Widget->GetFName());
				}
			}
		}
		// Clear existing entries (prevents stale keys).
		WidgetBP->Modify();
		GuidMap->Reset();

		int32 NumReused = 0;
		int32 NumCreated = 0;

		for (const FName& VarName : WidgetNames)
		{
			if (const FGuid* ExistingGuid = OldMap.Find(VarName))
			{
				GuidMap->Add(VarName, *ExistingGuid);
				++NumReused;
			}
			else
			{
				GuidMap->Add(VarName, FGuid::NewGuid());
				++NumCreated;
			}
		}

		WidgetBP->MarkPackageDirty();

		if (bLogChanges)
		{
			UE_LOG(LogTemp, Display, TEXT("[T66WidgetLayoutTools] Rebuilt GUID map for %s | Widgets=%d | Reused=%d | New=%d"),
				*GetNameSafe(WidgetBP), WidgetNames.Num(), NumReused, NumCreated);
		}

		// IMPORTANT:
		// Do NOT call MarkBlueprintAsStructurallyModified here.
		// We want the caller to do that exactly once after all WidgetTree edits are complete.
		return true;
	}

	// ---------------------------------------------------------------------
	// Legacy targeted helper (kept for now; contract tool will absorb it)
	// ---------------------------------------------------------------------

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

		// ✅ Repair/add-only: if root exists, do NOT touch layout
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

	static void GatherSelectedWidgetBlueprints(TArray<UWidgetBlueprint*>& OutWidgetBPs)
	{
		OutWidgetBPs.Reset();

		FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

		TArray<FAssetData> SelectedAssets;
		CBModule.Get().GetSelectedAssets(SelectedAssets);

		for (const FAssetData& AssetData : SelectedAssets)
		{
			UObject* Obj = AssetData.GetAsset();
			if (!Obj)
			{
				continue;
			}

			if (UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(Obj))
			{
				OutWidgetBPs.Add(WidgetBP);
			}
		}
	}

	static void ApplyContractsToWidgetBlueprints(const TArray<UWidgetBlueprint*>& WidgetBPs, const bool bForceOverride, const TCHAR* LogLabel)
	{
		if (WidgetBPs.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66WidgetLayoutTools] %s: No Widget Blueprints selected."), LogLabel);
			return;
		}

		int32 NumConsidered = 0;
		int32 NumStamped = 0;
		int32 NumSaved = 0;

		for (UWidgetBlueprint* WidgetBP : WidgetBPs)
		{
			if (!WidgetBP)
			{
				continue;
			}

			NumConsidered++;


			// Force mode should start from a truly empty widget tree.
			if (bForceOverride)
			{
				ForceReplaceWidgetTree(WidgetBP);
			}

			// Repair stale/missing WidgetVariableNameToGuidMap entries BEFORE we modify the WidgetTree.
			HardRebuildWidgetVariableNameGuidMap(WidgetBP, false);

			const bool bApplied = T66WidgetLayoutRecipes::TryApplyRecipe(WidgetBP, bForceOverride);

			// Repair stale/missing WidgetVariableNameToGuidMap entries AFTER we add/remove variable widgets.
			HardRebuildWidgetVariableNameGuidMap(WidgetBP, false);

			if (!bApplied)
			{
				continue;
			}

			// Let the editor know the widget tree shape changed.
			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBP);

			NumStamped++;

			// Compile + save
			UBlueprintEditorLibrary::CompileBlueprint(WidgetBP);

			WidgetBP->MarkPackageDirty();
			const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(WidgetBP, /*bOnlyIfIsDirty=*/true);
			if (bSaved)
			{
				NumSaved++;
			}
		}

		UE_LOG(LogTemp, Display, TEXT("[T66WidgetLayoutTools] %s complete. Considered=%d | Stamped=%d | Saved=%d"),
			LogLabel, NumConsidered, NumStamped, NumSaved);
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

	// ✅ Ensure exposed variables exist (legacy repair/add only).
	// NOTE: Once all buttons are reparented to UT66InteractiveButtonWidgetBase, these become redundant.
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
	TArray<UWidgetBlueprint*> WidgetBPs;
	T66WidgetLayoutTools_Internal::GatherSelectedWidgetBlueprints(WidgetBPs);

	// Legacy entrypoint: keep behavior identical (force rebuild).
	T66WidgetLayoutTools_Internal::ApplyContractsToWidgetBlueprints(WidgetBPs, /*bForceOverride=*/true, TEXT("BuildMinimumLayouts"));
}

// -----------------------------------------------------------------------------
// NEW: Widget Contracts (Safe + Force)
// -----------------------------------------------------------------------------
//
// IMPORTANT:
// - Safe = add/repair only. No overwrites.
// - Force = delete + rebuild from recipe.
//
// These call into the same recipe engine; recipes decide what "contracts" mean for each widget.
//
void UT66WidgetLayoutToolsSubsystem::CreateOrRepairUIWidgetContracts_SelectedWidgets_Safe()
{
	TArray<UWidgetBlueprint*> WidgetBPs;
	T66WidgetLayoutTools_Internal::GatherSelectedWidgetBlueprints(WidgetBPs);

	T66WidgetLayoutTools_Internal::ApplyContractsToWidgetBlueprints(WidgetBPs, /*bForceOverride=*/false, TEXT("WidgetContracts(Safe)"));
}

void UT66WidgetLayoutToolsSubsystem::CreateOrRepairUIWidgetContracts_SelectedWidgets_ForceOverwrite()
{
	TArray<UWidgetBlueprint*> WidgetBPs;
	T66WidgetLayoutTools_Internal::GatherSelectedWidgetBlueprints(WidgetBPs);

	T66WidgetLayoutTools_Internal::ApplyContractsToWidgetBlueprints(WidgetBPs, /*bForceOverride=*/true, TEXT("WidgetContracts(Force)"));
}
