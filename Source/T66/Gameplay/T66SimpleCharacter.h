#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "T66SimpleCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AT66CompanionActor;

/**
 * Simple playable character with WASD movement and jumping.
 * No Blueprint dependencies - pure C++.
 */
UCLASS()
class T66_API AT66SimpleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AT66SimpleCharacter();

	UFUNCTION(BlueprintCallable, Category = "T66|Character")
	void SetBodyColor(FLinearColor InColor);

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void ApplySelectedHeroAppearance();
	void SpawnCompanion();
	// Input handlers
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJump();

	// Camera boom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Follow camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Visible body mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BodyMesh;

	// Input actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	// Dynamic material for color changes
	UPROPERTY()
	class UMaterialInstanceDynamic* BodyMaterial;

	// Spawned companion reference
	UPROPERTY()
	TWeakObjectPtr<AT66CompanionActor> SpawnedCompanion;
};
