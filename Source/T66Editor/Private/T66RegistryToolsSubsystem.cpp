#include "T66RegistryToolsSubsystem.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "EditorAssetLibrary.h"
#include "GameplayTagContainer.h"
#include "Modules/ModuleManager.h"
#include "Blueprint/UserWidget.h"

// ✅ Runtime registry asset types
#include "T66UISurfaceRegistryDataAsset.h"
#include "T66UIInputContextRegistryDA.h"

// ✅ Enhanced Input types
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"

void UT66RegistryToolsSubsystem::FillSurfaceRegistry()
{
	// ✅ Scan the entire project so it works regardless of folder layout
	const FString WidgetRootPath = TEXT("/Game");

	// ------------------------------------------------------------
	// 1) Find the Surface Registry DataAsset (by name)
	// ------------------------------------------------------------
	UT66UISurfaceRegistryDataAsset* Registry = nullptr;

	{
		FAssetRegistryModule& AssetRegistryModule =
			FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		FARFilter Filter;
		Filter.PackagePaths.Add(FName(TEXT("/Game")));
		Filter.ClassPaths.Add(UT66UISurfaceRegistryDataAsset::StaticClass()->GetClassPathName());
		Filter.bRecursivePaths = true;

		TArray<FAssetData> FoundAssets;
		AssetRegistry.GetAssets(Filter, FoundAssets);

		for (const FAssetData& Asset : FoundAssets)
		{
			if (Asset.AssetName == FName(TEXT("DA_UIRegistry_Surfaces")))
			{
				Registry = Cast<UT66UISurfaceRegistryDataAsset>(Asset.GetAsset());
				break;
			}
		}
	}

	if (!Registry)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66RegistryToolsSubsystem] Could not find DA_UIRegistry_Surfaces (UT66UISurfaceRegistryDataAsset)."));
		return;
	}

	// ------------------------------------------------------------
	// 2) Scan for widget blueprints in /Game
	// ------------------------------------------------------------
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> WidgetAssets;
	AssetRegistry.GetAssetsByPath(FName(*WidgetRootPath), WidgetAssets, /*bRecursive=*/true);

	TMap<FGameplayTag, TSoftClassPtr<UUserWidget>> NewMap;

	auto TryAddMapping =
		[&](const FAssetData& AssetData, const FString& AssetNameStr, const FString& WidgetPrefix, const FString& TagPrefix)
		{
			if (!AssetNameStr.StartsWith(WidgetPrefix))
			{
				return;
			}

			const FString Suffix = AssetNameStr.RightChop(WidgetPrefix.Len());
			const FString TagString = TagPrefix + Suffix;

			const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString), /*ErrorIfNotFound=*/false);
			if (!Tag.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("[T66RegistryToolsSubsystem] GameplayTag missing: %s (skipping)"), *TagString);
				return;
			}

			const FString ObjectPath = AssetData.ObjectPath.ToString();
			const FString GeneratedClassPath = ObjectPath + TEXT("_C");

			TSoftClassPtr<UUserWidget> SoftWidgetClass{ FSoftObjectPath(GeneratedClassPath) };
			NewMap.Add(Tag, SoftWidgetClass);
		};

	for (const FAssetData& AssetData : WidgetAssets)
	{
		const FString AssetNameStr = AssetData.AssetName.ToString();

		TryAddMapping(AssetData, AssetNameStr, TEXT("WBP_Screen_"), TEXT("UI.Screen."));
		TryAddMapping(AssetData, AssetNameStr, TEXT("WBP_Modal_"), TEXT("UI.Modal."));
		TryAddMapping(AssetData, AssetNameStr, TEXT("WBP_Ov_"), TEXT("UI.Overlay."));
		TryAddMapping(AssetData, AssetNameStr, TEXT("WBP_Tooltip_"), TEXT("UI.Tooltip."));
	}

	// ------------------------------------------------------------
	// 3) Write the map into the DataAsset + save it
	// ------------------------------------------------------------
	Registry->Modify();
	Registry->SurfaceTagToWidgetClass = NewMap;
	Registry->MarkPackageDirty();

	const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(Registry, /*bOnlyIfIsDirty=*/true);

	UE_LOG(LogTemp, Display, TEXT("[T66RegistryToolsSubsystem] Filled Surface Registry: %d entries | Saved=%s"),
		NewMap.Num(),
		bSaved ? TEXT("true") : TEXT("false"));
}

void UT66RegistryToolsSubsystem::FillInputContextRegistry()
{
	// ✅ Scan the entire project so it works regardless of folder layout
	const FString IMCRootPath = TEXT("/Game");

	// ------------------------------------------------------------
	// 1) Find the Input Context Registry DataAsset (by name)
	// ------------------------------------------------------------
	UT66UIInputContextRegistryDA* Registry = nullptr;

	{
		FAssetRegistryModule& AssetRegistryModule =
			FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		FARFilter Filter;
		Filter.PackagePaths.Add(FName(TEXT("/Game")));
		Filter.ClassPaths.Add(UT66UIInputContextRegistryDA::StaticClass()->GetClassPathName());
		Filter.bRecursivePaths = true;

		TArray<FAssetData> FoundAssets;
		AssetRegistry.GetAssets(Filter, FoundAssets);

		for (const FAssetData& Asset : FoundAssets)
		{
			if (Asset.AssetName == FName(TEXT("DA_T66UIInputContexts")))
			{
				Registry = Cast<UT66UIInputContextRegistryDA>(Asset.GetAsset());
				break;
			}
		}
	}

	if (!Registry)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66RegistryToolsSubsystem] Could not find DA_T66UIInputContexts (UT66UIInputContextRegistryDA)."));
		return;
	}

	// ------------------------------------------------------------
	// 2) Scan for IMC_UI_* assets and map them to UI.Input.*
	// ------------------------------------------------------------
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AllAssets;
	AssetRegistry.GetAssetsByPath(FName(*IMCRootPath), AllAssets, /*bRecursive=*/true);

	TMap<FGameplayTag, TSoftObjectPtr<UInputMappingContext>> NewMap;

	for (const FAssetData& AssetData : AllAssets)
	{
		const FString AssetNameStr = AssetData.AssetName.ToString();

		// ✅ Only Input Mapping Context assets
		if (AssetData.AssetClassPath != UInputMappingContext::StaticClass()->GetClassPathName())
		{
			continue;
		}

		// ✅ Only IMC_UI_* naming
		if (!AssetNameStr.StartsWith(TEXT("IMC_UI_")))
		{
			continue;
		}

		const FString Suffix = AssetNameStr.RightChop(7); // "IMC_UI_" length
		const FString TagString = TEXT("UI.Input.") + Suffix;

		const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString), /*ErrorIfNotFound=*/false);
		if (!Tag.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66RegistryToolsSubsystem] GameplayTag missing: %s (skipping)"), *TagString);
			continue;
		}

		const FString ObjectPath = AssetData.ObjectPath.ToString();

		TSoftObjectPtr<UInputMappingContext> SoftIMC{ FSoftObjectPath(ObjectPath) };
		NewMap.Add(Tag, SoftIMC);
	}

	// ------------------------------------------------------------
	// 3) Write the map into the DataAsset + save it
	// ------------------------------------------------------------
	Registry->Modify();
	Registry->InputContextTagToIMC = NewMap;
	Registry->MarkPackageDirty();

	const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(Registry, /*bOnlyIfIsDirty=*/true);

	UE_LOG(LogTemp, Display, TEXT("[T66RegistryToolsSubsystem] Filled Input Context Registry: %d entries | Saved=%s"),
		NewMap.Num(),
		bSaved ? TEXT("true") : TEXT("false"));
}

void UT66RegistryToolsSubsystem::ApplyDefaultUIKeybinds()
{
	// ✅ Scan the entire project
	const FString RootPath = TEXT("/Game");

	// ------------------------------------------------------------
	// 1) Gather UI Input Actions (IA_UI_*)
	// ------------------------------------------------------------
	TMap<FString, UInputAction*> UIActionsByName;

	{
		FAssetRegistryModule& AssetRegistryModule =
			FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		TArray<FAssetData> AllAssets;
		AssetRegistry.GetAssetsByPath(FName(*RootPath), AllAssets, /*bRecursive=*/true);

		for (const FAssetData& AssetData : AllAssets)
		{
			// Only InputAction assets
			if (AssetData.AssetClassPath != UInputAction::StaticClass()->GetClassPathName())
			{
				continue;
			}

			const FString AssetNameStr = AssetData.AssetName.ToString();

			// Only IA_UI_* actions
			if (!AssetNameStr.StartsWith(TEXT("IA_UI_")))
			{
				continue;
			}

			if (UInputAction* Action = Cast<UInputAction>(AssetData.GetAsset()))
			{
				UIActionsByName.Add(AssetNameStr, Action);
			}
		}
	}

	auto GetAction = [&](const FString& Name) -> UInputAction*
		{
			if (UInputAction** Found = UIActionsByName.Find(Name))
			{
				return *Found;
			}

			UE_LOG(LogTemp, Warning, TEXT("[T66RegistryToolsSubsystem] Missing InputAction asset: %s"), *Name);
			return nullptr;
		};

	// Required actions (based on what you showed)
	UInputAction* IA_Confirm = GetAction(TEXT("IA_UI_Confirm"));
	UInputAction* IA_Cancel = GetAction(TEXT("IA_UI_Cancel"));
	UInputAction* IA_Back = GetAction(TEXT("IA_UI_Back"));
	UInputAction* IA_Pause = GetAction(TEXT("IA_UI_Pause"));
	UInputAction* IA_TabLeft = GetAction(TEXT("IA_UI_TabLeft"));
	UInputAction* IA_TabRight = GetAction(TEXT("IA_UI_TabRight"));
	UInputAction* IA_NavUp = GetAction(TEXT("IA_UI_NavUp"));
	UInputAction* IA_NavDown = GetAction(TEXT("IA_UI_NavDown"));
	UInputAction* IA_NavLeft = GetAction(TEXT("IA_UI_NavLeft"));
	UInputAction* IA_NavRight = GetAction(TEXT("IA_UI_NavRight"));

	// ------------------------------------------------------------
	// 2) Find IMC_UI_* contexts
	// ------------------------------------------------------------
	TArray<UInputMappingContext*> UIIMCs;

	{
		FAssetRegistryModule& AssetRegistryModule =
			FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		TArray<FAssetData> AllAssets;
		AssetRegistry.GetAssetsByPath(FName(*RootPath), AllAssets, /*bRecursive=*/true);

		for (const FAssetData& AssetData : AllAssets)
		{
			if (AssetData.AssetClassPath != UInputMappingContext::StaticClass()->GetClassPathName())
			{
				continue;
			}

			const FString AssetNameStr = AssetData.AssetName.ToString();

			if (!AssetNameStr.StartsWith(TEXT("IMC_UI_")))
			{
				continue;
			}

			if (UInputMappingContext* IMC = Cast<UInputMappingContext>(AssetData.GetAsset()))
			{
				UIIMCs.Add(IMC);
			}
		}
	}

	if (UIIMCs.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[T66RegistryToolsSubsystem] No IMC_UI_* assets found. Nothing to apply."));
		return;
	}

	// ------------------------------------------------------------
	// 3) Apply default keybinds to each IMC_UI_*
	//    (Keyboard + Gamepad = Steam Deck compatible)
	// ------------------------------------------------------------
	auto ClearAndMapKeys = [&](UInputMappingContext* IMC, UInputAction* Action, const TArray<FKey>& Keys)
		{
			if (!IMC || !Action)
			{
				return;
			}

			// Remove all existing mappings for this action before adding defaults
			IMC->UnmapAllKeysFromAction(Action);

			for (const FKey& Key : Keys)
			{
				IMC->MapKey(Action, Key);
			}
		};

	int32 NumContextsSaved = 0;

	for (UInputMappingContext* IMC : UIIMCs)
	{
		if (!IMC)
		{
			continue;
		}

		IMC->Modify();

		// ✅ Confirm
		ClearAndMapKeys(IMC, IA_Confirm, {
			EKeys::Enter,
			EKeys::SpaceBar,
			EKeys::Gamepad_FaceButton_Bottom  // A (Xbox) / Cross (PS)
			});

		// ✅ Cancel
		ClearAndMapKeys(IMC, IA_Cancel, {
			EKeys::Escape,
			EKeys::Gamepad_FaceButton_Right   // B (Xbox) / Circle (PS)
			});

		// ✅ Back (optional extra “go back” action)
		ClearAndMapKeys(IMC, IA_Back, {
			EKeys::BackSpace,
			EKeys::Gamepad_FaceButton_Right   // B
			});

		// ✅ Pause (menu/start)
		ClearAndMapKeys(IMC, IA_Pause, {
			EKeys::Escape,
			EKeys::Gamepad_Special_Right      // Start/Menu
			});

		// ✅ Tabs
		ClearAndMapKeys(IMC, IA_TabLeft, {
			EKeys::Q,
			EKeys::Gamepad_LeftShoulder
			});

		ClearAndMapKeys(IMC, IA_TabRight, {
			EKeys::E,
			EKeys::Gamepad_RightShoulder
			});

		// ✅ UI Navigation (keyboard arrows + gamepad d-pad)
		ClearAndMapKeys(IMC, IA_NavUp, {
			EKeys::Up,
			EKeys::Gamepad_DPad_Up
			});

		ClearAndMapKeys(IMC, IA_NavDown, {
			EKeys::Down,
			EKeys::Gamepad_DPad_Down
			});

		ClearAndMapKeys(IMC, IA_NavLeft, {
			EKeys::Left,
			EKeys::Gamepad_DPad_Left
			});

		ClearAndMapKeys(IMC, IA_NavRight, {
			EKeys::Right,
			EKeys::Gamepad_DPad_Right
			});

		IMC->MarkPackageDirty();

		const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(IMC, /*bOnlyIfIsDirty=*/true);
		if (bSaved)
		{
			NumContextsSaved++;
		}

		UE_LOG(LogTemp, Display, TEXT("[T66RegistryToolsSubsystem] Applied default UI keybinds to: %s | Saved=%s"),
			*IMC->GetName(),
			bSaved ? TEXT("true") : TEXT("false"));
	}

	UE_LOG(LogTemp, Display, TEXT("[T66RegistryToolsSubsystem] ApplyDefaultUIKeybinds complete. Contexts updated: %d"),
		NumContextsSaved);
}
