#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "T66PreviewCapture.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class USpotLightComponent;
class UDirectionalLightComponent;

/**
 * Captures a 3D preview of a character/companion for display in UI
 * Similar to how Dota 2 renders hero previews
 */
UCLASS()
class T66_API AT66PreviewCapture : public AActor
{
	GENERATED_BODY()

public:
	AT66PreviewCapture();

	virtual void BeginPlay() override;

	// Set up the capture for a hero preview
	UFUNCTION(BlueprintCallable, Category = "T66|Preview")
	void SetupHeroPreview(FLinearColor HeroColor);

	// Set up the capture for a companion preview
	UFUNCTION(BlueprintCallable, Category = "T66|Preview")
	void SetupCompanionPreview(FLinearColor CompanionColor);

	// Get the render target for UI display
	UFUNCTION(BlueprintCallable, Category = "T66|Preview")
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

	// Rotate the preview model
	UFUNCTION(BlueprintCallable, Category = "T66|Preview")
	void RotatePreview(float DeltaYaw);

protected:
	// Scene capture component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capture")
	USceneCaptureComponent2D* SceneCapture;

	// The render target we capture to
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Capture")
	UTextureRenderTarget2D* RenderTarget;

	// Root for the preview subject
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
	USceneComponent* PreviewRoot;

	// Hero cylinder mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
	UStaticMeshComponent* HeroMesh;

	// Companion cube mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
	UStaticMeshComponent* CompanionMesh;

	// Key light (main directional)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
	USpotLightComponent* KeyLight;

	// Fill light (softer, opposite side)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
	USpotLightComponent* FillLight;

	// Rim light (backlight for silhouette)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
	USpotLightComponent* RimLight;

	// Dynamic material for hero
	UPROPERTY()
	UMaterialInstanceDynamic* HeroMaterial;

	// Dynamic material for companion
	UPROPERTY()
	UMaterialInstanceDynamic* CompanionMaterial;

private:
	void CreateRenderTarget();
	void SetupLighting();
};
