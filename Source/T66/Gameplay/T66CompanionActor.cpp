#include "T66CompanionActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AT66CompanionActor::AT66CompanionActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create cube mesh
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	RootComponent = CubeMesh;
	CubeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Load cube mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshFinder.Succeeded())
	{
		CubeMesh->SetStaticMesh(CubeMeshFinder.Object);
	}

	// Scale to a reasonable size
	SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
}

void AT66CompanionActor::BeginPlay()
{
	Super::BeginPlay();

	// Create dynamic material for color changes
	if (CubeMesh && CubeMesh->GetMaterial(0))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(CubeMesh->GetMaterial(0), this);
		CubeMesh->SetMaterial(0, DynamicMaterial);
	}

	// Find the player to follow
	if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		SetFollowTarget(PlayerCharacter);
	}

	UE_LOG(LogTemp, Display, TEXT("[T66CompanionActor] Companion spawned"));
}

void AT66CompanionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FollowTarget.IsValid())
	{
		// Try to find player again
		if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			SetFollowTarget(PlayerCharacter);
		}
		return;
	}

	// Calculate target position (behind and to the side of the player)
	const FVector TargetLocation = FollowTarget->GetActorLocation();
	const FRotator TargetRotation = FollowTarget->GetActorRotation();
	
	// Position behind and to the right of the player
	const FVector OffsetDirection = TargetRotation.RotateVector(FVector(-FollowDistance, FollowDistance * 0.5f, 0.0f));
	
	// Add bobbing motion
	BobTime += DeltaTime * BobSpeed;
	const float BobOffset = FMath::Sin(BobTime) * BobAmount;
	
	const FVector DesiredLocation = TargetLocation + OffsetDirection + FVector(0.0f, 0.0f, HoverHeight + BobOffset);

	// Smoothly interpolate to target position
	const FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DesiredLocation, DeltaTime, FollowSpeed);
	SetActorLocation(NewLocation);

	// Slowly rotate
	AddActorLocalRotation(FRotator(0.0f, DeltaTime * 45.0f, 0.0f));
}

void AT66CompanionActor::SetCompanionColor(FLinearColor InColor)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(FName(TEXT("BaseColor")), InColor);
	}
	
	// Also try setting the emissive color for better visibility
	if (CubeMesh)
	{
		// Create a simple colored material
		UMaterialInstanceDynamic* ColorMat = UMaterialInstanceDynamic::Create(
			CubeMesh->GetMaterial(0), this);
		if (ColorMat)
		{
			ColorMat->SetVectorParameterValue(FName(TEXT("BaseColor")), InColor);
			CubeMesh->SetMaterial(0, ColorMat);
			DynamicMaterial = ColorMat;
		}
	}
	
	UE_LOG(LogTemp, Display, TEXT("[T66CompanionActor] Set color to R:%.2f G:%.2f B:%.2f"), 
		InColor.R, InColor.G, InColor.B);
}

void AT66CompanionActor::SetFollowTarget(AActor* InTarget)
{
	FollowTarget = InTarget;
	UE_LOG(LogTemp, Display, TEXT("[T66CompanionActor] Following target: %s"), 
		InTarget ? *InTarget->GetName() : TEXT("None"));
}
