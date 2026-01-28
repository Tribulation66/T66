#include "T66PreviewCapture.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AT66PreviewCapture::AT66PreviewCapture()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Create preview root (this will hold and rotate the preview subject)
	PreviewRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PreviewRoot"));
	PreviewRoot->SetupAttachment(Root);
	PreviewRoot->SetRelativeLocation(FVector(200.0f, 0.0f, 0.0f)); // In front of camera

	// Create hero mesh (cylinder - tall oval shape)
	HeroMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeroMesh"));
	HeroMesh->SetupAttachment(PreviewRoot);
	HeroMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	HeroMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.8f)); // Tall oval
	HeroMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeroMesh->SetVisibility(false);
	HeroMesh->bOnlyOwnerSee = true; // Only captured, not seen in main view

	// Load cylinder mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderFinder.Succeeded())
	{
		HeroMesh->SetStaticMesh(CylinderFinder.Object);
	}

	// Create companion mesh (cube)
	CompanionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CompanionMesh"));
	CompanionMesh->SetupAttachment(PreviewRoot);
	CompanionMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	CompanionMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CompanionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CompanionMesh->SetVisibility(false);
	CompanionMesh->bOnlyOwnerSee = true;

	// Load cube mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		CompanionMesh->SetStaticMesh(CubeFinder.Object);
	}

	// Create scene capture component
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(Root);
	SceneCapture->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f)); // Slightly above center
	SceneCapture->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f)); // Looking forward (+X)
	
	// Configure capture settings for clean preview
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalToneCurveHDR;
	SceneCapture->bCaptureEveryFrame = true;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->FOVAngle = 30.0f; // Narrow FOV for less distortion

	// Create lights
	KeyLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("KeyLight"));
	KeyLight->SetupAttachment(Root);
	
	FillLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("FillLight"));
	FillLight->SetupAttachment(Root);
	
	RimLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("RimLight"));
	RimLight->SetupAttachment(Root);
}

void AT66PreviewCapture::BeginPlay()
{
	Super::BeginPlay();

	CreateRenderTarget();
	SetupLighting();

	// Add meshes to show only list for scene capture
	SceneCapture->ShowOnlyActors.Add(this);

	UE_LOG(LogTemp, Display, TEXT("[T66PreviewCapture] Preview capture initialized"));
}

void AT66PreviewCapture::CreateRenderTarget()
{
	// Create render target texture
	RenderTarget = NewObject<UTextureRenderTarget2D>(this, TEXT("PreviewRenderTarget"));
	RenderTarget->InitCustomFormat(512, 512, PF_B8G8R8A8, false);
	RenderTarget->ClearColor = FLinearColor(0.02f, 0.02f, 0.05f, 1.0f); // Dark background
	RenderTarget->UpdateResourceImmediate();

	// Assign to scene capture
	SceneCapture->TextureTarget = RenderTarget;

	UE_LOG(LogTemp, Display, TEXT("[T66PreviewCapture] Created 512x512 render target"));
}

void AT66PreviewCapture::SetupLighting()
{
	// Key light - main illumination from front-right-above
	KeyLight->SetRelativeLocation(FVector(100.0f, 100.0f, 150.0f));
	KeyLight->SetRelativeRotation(FRotator(-30.0f, -135.0f, 0.0f));
	KeyLight->SetIntensity(8000.0f);
	KeyLight->SetAttenuationRadius(500.0f);
	KeyLight->SetOuterConeAngle(45.0f);
	KeyLight->SetInnerConeAngle(30.0f);
	KeyLight->SetLightColor(FLinearColor(1.0f, 0.95f, 0.9f)); // Slightly warm

	// Fill light - softer, from front-left
	FillLight->SetRelativeLocation(FVector(80.0f, -80.0f, 80.0f));
	FillLight->SetRelativeRotation(FRotator(-15.0f, 135.0f, 0.0f));
	FillLight->SetIntensity(3000.0f);
	FillLight->SetAttenuationRadius(400.0f);
	FillLight->SetOuterConeAngle(60.0f);
	FillLight->SetInnerConeAngle(40.0f);
	FillLight->SetLightColor(FLinearColor(0.8f, 0.85f, 1.0f)); // Slightly cool

	// Rim light - backlight for silhouette definition
	RimLight->SetRelativeLocation(FVector(300.0f, 0.0f, 100.0f));
	RimLight->SetRelativeRotation(FRotator(-10.0f, 180.0f, 0.0f));
	RimLight->SetIntensity(5000.0f);
	RimLight->SetAttenuationRadius(600.0f);
	RimLight->SetOuterConeAngle(50.0f);
	RimLight->SetInnerConeAngle(35.0f);
	RimLight->SetLightColor(FLinearColor(0.9f, 0.95f, 1.0f)); // Cool white

	UE_LOG(LogTemp, Display, TEXT("[T66PreviewCapture] Set up 3-point lighting"));
}

void AT66PreviewCapture::SetupHeroPreview(FLinearColor HeroColor)
{
	// Hide companion, show hero
	CompanionMesh->SetVisibility(false);
	HeroMesh->SetVisibility(true);

	// Create dynamic material with hero color
	if (!HeroMaterial)
	{
		UMaterialInterface* BaseMat = HeroMesh->GetMaterial(0);
		if (BaseMat)
		{
			HeroMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			HeroMesh->SetMaterial(0, HeroMaterial);
		}
	}

	if (HeroMaterial)
	{
		HeroMaterial->SetVectorParameterValue(FName(TEXT("BaseColor")), HeroColor);
	}

	// Reset rotation
	PreviewRoot->SetRelativeRotation(FRotator::ZeroRotator);

	UE_LOG(LogTemp, Display, TEXT("[T66PreviewCapture] Set up hero preview with color R:%.2f G:%.2f B:%.2f"),
		HeroColor.R, HeroColor.G, HeroColor.B);
}

void AT66PreviewCapture::SetupCompanionPreview(FLinearColor CompanionColor)
{
	// Hide hero, show companion
	HeroMesh->SetVisibility(false);
	CompanionMesh->SetVisibility(true);

	// Create dynamic material with companion color
	if (!CompanionMaterial)
	{
		UMaterialInterface* BaseMat = CompanionMesh->GetMaterial(0);
		if (BaseMat)
		{
			CompanionMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			CompanionMesh->SetMaterial(0, CompanionMaterial);
		}
	}

	if (CompanionMaterial)
	{
		CompanionMaterial->SetVectorParameterValue(FName(TEXT("BaseColor")), CompanionColor);
	}

	// Reset rotation
	PreviewRoot->SetRelativeRotation(FRotator::ZeroRotator);

	UE_LOG(LogTemp, Display, TEXT("[T66PreviewCapture] Set up companion preview with color R:%.2f G:%.2f B:%.2f"),
		CompanionColor.R, CompanionColor.G, CompanionColor.B);
}

void AT66PreviewCapture::RotatePreview(float DeltaYaw)
{
	FRotator CurrentRotation = PreviewRoot->GetRelativeRotation();
	CurrentRotation.Yaw += DeltaYaw;
	PreviewRoot->SetRelativeRotation(CurrentRotation);
}
