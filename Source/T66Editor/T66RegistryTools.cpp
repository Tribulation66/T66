#include "T66RegistryTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "EditorAssetLibrary.h"
#include "GameplayTagContainer.h"
#include "Modules/ModuleManager.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Package.h"

// ✅ Runtime registry asset types
#include "T66UISurfaceRegistryDataAsset.h"
#include "T66UIInputContextRegistryDA.h"
#include "T66UIThemeDA.h"

// ✅ Enhanced Input types
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"

void UT66RegistryTools::FillSurfaceRegistry()
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
		UE_LOG(LogTemp, Error, TEXT("[T66RegistryTools] Could not find DA_UIRegistry_Surfaces (UT66UISurfaceRegistryDataAsset)."));
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
				UE_LOG(LogTemp, Warning, TEXT("[T66RegistryTools] GameplayTag missing: %s (skipping)"), *TagString);
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

	UE_LOG(LogTemp, Display, TEXT("[T66RegistryTools] Filled Surface Registry: %d entries | Saved=%s"),
		NewMap.Num(),
		bSaved ? TEXT("true") : TEXT("false"));
}

void UT66RegistryTools::FillInputContextRegistry()
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
		UE_LOG(LogTemp, Error, TEXT("[T66RegistryTools] Could not find DA_T66UIInputContexts (UT66UIInputContextRegistryDA)."));
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
			UE_LOG(LogTemp, Warning, TEXT("[T66RegistryTools] GameplayTag missing: %s (skipping)"), *TagString);
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

	UE_LOG(LogTemp, Display, TEXT("[T66RegistryTools] Filled Input Context Registry: %d entries | Saved=%s"),
		NewMap.Num(),
		bSaved ? TEXT("true") : TEXT("false"));
}

void UT66RegistryTools::ApplyDefaultUIKeybinds()
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

			UE_LOG(LogTemp, Warning, TEXT("[T66RegistryTools] Missing InputAction asset: %s"), *Name);
			return nullptr;
		};

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
		UE_LOG(LogTemp, Warning, TEXT("[T66RegistryTools] No IMC_UI_* assets found. Nothing to apply."));
		return;
	}

	// ------------------------------------------------------------
	// 3) Apply default keybinds to each IMC_UI_*
	// ------------------------------------------------------------
	auto ClearAndMapKeys = [&](UInputMappingContext* IMC, UInputAction* Action, const TArray<FKey>& Keys)
		{
			if (!IMC || !Action)
			{
				return;
			}

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

		ClearAndMapKeys(IMC, IA_Confirm, {
			EKeys::Enter,
			EKeys::SpaceBar,
			EKeys::Gamepad_FaceButton_Bottom
			});

		ClearAndMapKeys(IMC, IA_Cancel, {
			EKeys::Escape,
			EKeys::Gamepad_FaceButton_Right
			});

		ClearAndMapKeys(IMC, IA_Back, {
			EKeys::BackSpace,
			EKeys::Gamepad_FaceButton_Right
			});

		ClearAndMapKeys(IMC, IA_Pause, {
			EKeys::Escape,
			EKeys::Gamepad_Special_Right
			});

		ClearAndMapKeys(IMC, IA_TabLeft, {
			EKeys::Q,
			EKeys::Gamepad_LeftShoulder
			});

		ClearAndMapKeys(IMC, IA_TabRight, {
			EKeys::E,
			EKeys::Gamepad_RightShoulder
			});

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

		UE_LOG(LogTemp, Display, TEXT("[T66RegistryTools] Applied default UI keybinds to: %s | Saved=%s"),
			*IMC->GetName(),
			bSaved ? TEXT("true") : TEXT("false"));
	}

	UE_LOG(LogTemp, Display, TEXT("[T66RegistryTools] ApplyDefaultUIKeybinds complete. Contexts updated: %d"),
		NumContextsSaved);
}

void UT66RegistryTools::CreateOrRepairUIThemeAssets()
{
	// ------------------------------------------------------------
	// Goal:
	// - SINGLE SOURCE OF TRUTH for theme assets location
	// - Ensure the 6 required theme DataAssets exist
	// - Move legacy duplicates into the canonical folder (when safe)
	// - Auto-fill safe defaults (only fills missing keys; won't overwrite your custom values)
	// ------------------------------------------------------------

	// ✅ Canonical folder (single source of truth)
	const FString CanonicalThemeFolder = TEXT("/Game/Tribulation66/Content/UI/DataAssets/Theme");
	UEditorAssetLibrary::MakeDirectory(CanonicalThemeFolder);

	// Legacy folders we previously used (we will migrate from these if found)
	const TArray<FString> LegacyFolders =
	{
		TEXT("/Game/Tribulation66/UI/Theme"),
		TEXT("/Game/UI/Theme"),
		TEXT("/Game/Tribulation66/Content/UI/Theme")
	};

	const TArray<FString> RequiredAssetNames =
	{
		TEXT("DA_UITheme_Root"),
		TEXT("DA_UITheme_Text"),
		TEXT("DA_UITheme_Button"),
		TEXT("DA_UITheme_Panel"),
		TEXT("DA_UITheme_Background"),
		TEXT("DA_UITheme_Audio")
	};

	int32 NumCreated = 0;
	int32 NumMoved = 0;
	int32 NumSaved = 0;

	auto EnsureColor = [](UT66UIThemeDA* Theme, const FName Key, const FLinearColor Value)
		{
			if (Theme && !Theme->ColorTokens.Contains(Key))
			{
				Theme->ColorTokens.Add(Key, Value);
			}
		};

	auto EnsureFloat = [](UT66UIThemeDA* Theme, const FName Key, const float Value)
		{
			if (Theme && !Theme->FloatTokens.Contains(Key))
			{
				Theme->FloatTokens.Add(Key, Value);
			}
		};

	auto EnsureMargin = [](UT66UIThemeDA* Theme, const FName Key, const FMargin Value)
		{
			if (Theme && !Theme->MarginTokens.Contains(Key))
			{
				Theme->MarginTokens.Add(Key, Value);
			}
		};

	auto EnsureVector2 = [](UT66UIThemeDA* Theme, const FName Key, const FVector2D Value)
		{
			if (Theme && !Theme->Vector2Tokens.Contains(Key))
			{
				Theme->Vector2Tokens.Add(Key, Value);
			}
		};

	for (const FString& AssetName : RequiredAssetNames)
	{
		// Canonical package/object path
		const FString CanonicalPackagePath = CanonicalThemeFolder + TEXT("/") + AssetName;
		const FString CanonicalObjectPath = CanonicalPackagePath + TEXT(".") + AssetName;

		UT66UIThemeDA* ThemeDA = nullptr;

		// 1) If it exists in canonical, load it
		if (UEditorAssetLibrary::DoesAssetExist(CanonicalObjectPath))
		{
			ThemeDA = Cast<UT66UIThemeDA>(UEditorAssetLibrary::LoadAsset(CanonicalObjectPath));
		}
		else
		{
			// 2) Otherwise, try to find + move from legacy folders
			bool bMovedFromLegacy = false;

			for (const FString& LegacyFolder : LegacyFolders)
			{
				const FString LegacyPackagePath = LegacyFolder + TEXT("/") + AssetName;
				const FString LegacyObjectPath = LegacyPackagePath + TEXT(".") + AssetName;

				if (!UEditorAssetLibrary::DoesAssetExist(LegacyObjectPath))
				{
					continue;
				}

				// Move it into canonical (rename asset path)
				const bool bRenamed = UEditorAssetLibrary::RenameAsset(LegacyPackagePath, CanonicalPackagePath);
				if (bRenamed)
				{
					NumMoved++;
					bMovedFromLegacy = true;
					break;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[T66RegistryTools] Found legacy theme asset but failed to move: %s"), *LegacyObjectPath);
				}
			}

			// After move attempt, try loading again from canonical
			if (bMovedFromLegacy && UEditorAssetLibrary::DoesAssetExist(CanonicalObjectPath))
			{
				ThemeDA = Cast<UT66UIThemeDA>(UEditorAssetLibrary::LoadAsset(CanonicalObjectPath));
			}
		}

		// 3) If still missing, create it fresh in canonical folder
		if (!ThemeDA)
		{
			UPackage* Package = CreatePackage(*CanonicalPackagePath);
			if (!Package)
			{
				UE_LOG(LogTemp, Error, TEXT("[T66RegistryTools] Failed to create package for %s"), *CanonicalPackagePath);
				continue;
			}

			ThemeDA = NewObject<UT66UIThemeDA>(Package, UT66UIThemeDA::StaticClass(), *AssetName, RF_Public | RF_Standalone);
			if (!ThemeDA)
			{
				UE_LOG(LogTemp, Error, TEXT("[T66RegistryTools] Failed to create theme asset object: %s"), *AssetName);
				continue;
			}

			FAssetRegistryModule::AssetCreated(ThemeDA);
			ThemeDA->MarkPackageDirty();
			NumCreated++;
		}

		// 4) Fill safe defaults WITHOUT overwriting your values
		ThemeDA->Modify();

		// Basic typography defaults
		if (ThemeDA->BodyText.FontSize <= 0)
		{
			ThemeDA->BodyText.FontSize = 18;
		}
		if (ThemeDA->TitleText.FontSize <= 0)
		{
			ThemeDA->TitleText.FontSize = 28;
		}

		// Neutral starter tokens
		EnsureColor(ThemeDA, FName(TEXT("Text.Primary")), FLinearColor(1.f, 1.f, 1.f, 1.f));
		EnsureColor(ThemeDA, FName(TEXT("Text.Muted")), FLinearColor(0.70f, 0.70f, 0.70f, 1.f));
		EnsureColor(ThemeDA, FName(TEXT("Panel.Bg")), FLinearColor(0.06f, 0.06f, 0.06f, 1.f));
		EnsureColor(ThemeDA, FName(TEXT("Panel.Border")), FLinearColor(0.20f, 0.20f, 0.20f, 1.f));
		EnsureColor(ThemeDA, FName(TEXT("Accent.Primary")), FLinearColor(0.10f, 0.60f, 1.00f, 1.f));

		EnsureFloat(ThemeDA, FName(TEXT("Opacity.Disabled")), 0.40f);
		EnsureFloat(ThemeDA, FName(TEXT("Anim.Speed")), 1.00f);
		EnsureFloat(ThemeDA, FName(TEXT("Button.MinWidth")), 260.0f);

		EnsureMargin(ThemeDA, FName(TEXT("Padding.Button")), FMargin(16.f, 10.f, 16.f, 10.f));
		EnsureMargin(ThemeDA, FName(TEXT("Padding.Panel")), FMargin(16.f, 16.f, 16.f, 16.f));

		EnsureVector2(ThemeDA, FName(TEXT("Icon.Size.S")), FVector2D(24.f, 24.f));
		EnsureVector2(ThemeDA, FName(TEXT("Button.MinSize")), FVector2D(260.f, 48.f));

		ThemeDA->MarkPackageDirty();

		const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(ThemeDA, /*bOnlyIfIsDirty=*/true);
		if (bSaved)
		{
			NumSaved++;
		}

		UE_LOG(LogTemp, Display, TEXT("[T66RegistryTools] Theme asset processed: %s | Saved=%s"),
			*AssetName,
			bSaved ? TEXT("true") : TEXT("false"));
	}

	UE_LOG(LogTemp, Display, TEXT("[T66RegistryTools] CreateOrRepairUIThemeAssets complete. Created=%d | Moved=%d | Saved=%d | CanonicalFolder=%s"),
		NumCreated,
		NumMoved,
		NumSaved,
		*CanonicalThemeFolder);
}
