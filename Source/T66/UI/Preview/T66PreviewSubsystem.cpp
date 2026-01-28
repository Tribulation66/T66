#include "T66PreviewSubsystem.h"
#include "T66PreviewCapture.h"
#include "Engine/World.h"
#include "Engine/TextureRenderTarget2D.h"

void UT66PreviewSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Defer spawning to ensure world is ready
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick([this]()
		{
			SpawnPreviewCaptures();
		});
	}
	
	UE_LOG(LogTemp, Display, TEXT("[T66PreviewSubsystem] Initialized"));
}

void UT66PreviewSubsystem::Deinitialize()
{
	if (HeroCapture)
	{
		HeroCapture->Destroy();
		HeroCapture = nullptr;
	}
	
	if (CompanionCapture)
	{
		CompanionCapture->Destroy();
		CompanionCapture = nullptr;
	}
	
	UE_LOG(LogTemp, Display, TEXT("[T66PreviewSubsystem] Deinitialized"));
	
	Super::Deinitialize();
}

void UT66PreviewSubsystem::SpawnPreviewCaptures()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// Spawn hero preview capture
	FVector HeroLocation(PREVIEW_LOCATION_X, PREVIEW_LOCATION_Y, PREVIEW_LOCATION_Z);
	HeroCapture = World->SpawnActor<AT66PreviewCapture>(
		AT66PreviewCapture::StaticClass(),
		HeroLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);
	
	if (HeroCapture)
	{
		UE_LOG(LogTemp, Display, TEXT("[T66PreviewSubsystem] Spawned hero preview capture at %.0f, %.0f, %.0f"),
			HeroLocation.X, HeroLocation.Y, HeroLocation.Z);
	}
	
	// Spawn companion preview capture (offset from hero)
	FVector CompanionLocation(PREVIEW_LOCATION_X, PREVIEW_LOCATION_Y + 500.0f, PREVIEW_LOCATION_Z);
	CompanionCapture = World->SpawnActor<AT66PreviewCapture>(
		AT66PreviewCapture::StaticClass(),
		CompanionLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);
	
	if (CompanionCapture)
	{
		UE_LOG(LogTemp, Display, TEXT("[T66PreviewSubsystem] Spawned companion preview capture"));
	}
}

void UT66PreviewSubsystem::ShowHeroPreview(FLinearColor HeroColor)
{
	if (HeroCapture)
	{
		HeroCapture->SetupHeroPreview(HeroColor);
	}
}

void UT66PreviewSubsystem::ShowCompanionPreview(FLinearColor CompanionColor)
{
	if (CompanionCapture)
	{
		CompanionCapture->SetupCompanionPreview(CompanionColor);
	}
}

UTextureRenderTarget2D* UT66PreviewSubsystem::GetHeroRenderTarget() const
{
	return HeroCapture ? HeroCapture->GetRenderTarget() : nullptr;
}

UTextureRenderTarget2D* UT66PreviewSubsystem::GetCompanionRenderTarget() const
{
	return CompanionCapture ? CompanionCapture->GetRenderTarget() : nullptr;
}

void UT66PreviewSubsystem::RotateHeroPreview(float DeltaYaw)
{
	if (HeroCapture)
	{
		HeroCapture->RotatePreview(DeltaYaw);
	}
}

void UT66PreviewSubsystem::RotateCompanionPreview(float DeltaYaw)
{
	if (CompanionCapture)
	{
		CompanionCapture->RotatePreview(DeltaYaw);
	}
}
