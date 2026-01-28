#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "T66PreviewSubsystem.generated.h"

class AT66PreviewCapture;
class UTextureRenderTarget2D;

/**
 * Manages the 3D preview capture system for hero/companion selection
 */
UCLASS()
class T66_API UT66PreviewSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Set up preview for a hero
	UFUNCTION(BlueprintCallable, Category = "T66|Preview")
	void ShowHeroPreview(FLinearColor HeroColor);

	// Set up preview for a companion
	UFUNCTION(BlueprintCallable, Category = "T66|Preview")
	void ShowCompanionPreview(FLinearColor CompanionColor);

	// Get the hero preview render target
	UFUNCTION(BlueprintCallable, Category = "T66|Preview")
	UTextureRenderTarget2D* GetHeroRenderTarget() const;

	// Get the companion preview render target
	UFUNCTION(BlueprintCallable, Category = "T66|Preview")
	UTextureRenderTarget2D* GetCompanionRenderTarget() const;

	// Rotate hero preview
	UFUNCTION(BlueprintCallable, Category = "T66|Preview")
	void RotateHeroPreview(float DeltaYaw);

	// Rotate companion preview
	UFUNCTION(BlueprintCallable, Category = "T66|Preview")
	void RotateCompanionPreview(float DeltaYaw);

private:
	void SpawnPreviewCaptures();

	UPROPERTY()
	TObjectPtr<AT66PreviewCapture> HeroCapture;

	UPROPERTY()
	TObjectPtr<AT66PreviewCapture> CompanionCapture;

	// Location far from play area for preview captures
	static constexpr float PREVIEW_LOCATION_X = -10000.0f;
	static constexpr float PREVIEW_LOCATION_Y = -10000.0f;
	static constexpr float PREVIEW_LOCATION_Z = 0.0f;
};
