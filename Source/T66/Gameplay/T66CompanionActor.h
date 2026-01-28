#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "T66CompanionActor.generated.h"

/**
 * A companion cube that follows the player around
 */
UCLASS()
class T66_API AT66CompanionActor : public AActor
{
	GENERATED_BODY()

public:
	AT66CompanionActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "T66|Companion")
	void SetCompanionColor(FLinearColor InColor);

	UFUNCTION(BlueprintCallable, Category = "T66|Companion")
	void SetFollowTarget(AActor* InTarget);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CubeMesh;

	UPROPERTY(EditAnywhere, Category = "Following")
	float FollowSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Following")
	float FollowDistance = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Following")
	float HoverHeight = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Following")
	float BobSpeed = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Following")
	float BobAmount = 20.0f;

	UPROPERTY()
	TWeakObjectPtr<AActor> FollowTarget;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

	float BobTime = 0.0f;
};
