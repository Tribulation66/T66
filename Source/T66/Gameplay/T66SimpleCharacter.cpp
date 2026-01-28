#include "T66SimpleCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "T66CompanionActor.h"
#include "T66SelectionSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AT66SimpleCharacter::AT66SimpleCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create visible body mesh (cylinder shape)
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f)); // Center in capsule
	BodyMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.8f)); // Tall oval shape
	
	// Load cylinder mesh from engine
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CylinderMesh.Object);
	}

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AT66SimpleCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] BeginPlay started"));

	// Load input assets at runtime
	if (!DefaultMappingContext)
	{
		DefaultMappingContext = Cast<UInputMappingContext>(StaticLoadObject(UInputMappingContext::StaticClass(), nullptr, TEXT("/Game/Input/IMC_Default.IMC_Default")));
		UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Loaded IMC_Default: %s"), DefaultMappingContext ? TEXT("SUCCESS") : TEXT("FAILED"));
	}
	if (!MoveAction)
	{
		MoveAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Input/Actions/IA_Move.IA_Move")));
		UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Loaded IA_Move: %s"), MoveAction ? TEXT("SUCCESS") : TEXT("FAILED"));
	}
	if (!LookAction)
	{
		LookAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Input/Actions/IA_Look.IA_Look")));
		UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Loaded IA_Look: %s"), LookAction ? TEXT("SUCCESS") : TEXT("FAILED"));
	}
	if (!JumpAction)
	{
		JumpAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Input/Actions/IA_Jump.IA_Jump")));
		UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Loaded IA_Jump: %s"), JumpAction ? TEXT("SUCCESS") : TEXT("FAILED"));
	}

	// Add Input Mapping Context
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] PlayerController: %s"), PlayerController ? TEXT("FOUND") : TEXT("NULL"));
	
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] EnhancedInputSubsystem: %s"), Subsystem ? TEXT("FOUND") : TEXT("NULL"));
		
		if (Subsystem && DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Added mapping context successfully"));
		}
		
		// NOW bind the input actions (after they are loaded)
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Binding input actions in BeginPlay..."));
			
			if (MoveAction)
			{
				EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AT66SimpleCharacter::Move);
				UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Bound Move action"));
			}
			if (LookAction)
			{
				EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AT66SimpleCharacter::Look);
				UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Bound Look action"));
			}
			if (JumpAction)
			{
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AT66SimpleCharacter::StartJump);
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AT66SimpleCharacter::StopJump);
				UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Bound Jump action"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[T66SimpleCharacter] Could not get EnhancedInputComponent from PlayerController!"));
		}
	}
	
	// Apply hero appearance and spawn companion based on selection
	ApplySelectedHeroAppearance();
	SpawnCompanion();
	
	UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] BeginPlay completed"));
}

void AT66SimpleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] SetupPlayerInputComponent called"));

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] EnhancedInputComponent: %s"), EnhancedInputComponent ? TEXT("FOUND") : TEXT("NULL - using legacy input?"));

	if (EnhancedInputComponent)
	{
		// Moving
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AT66SimpleCharacter::Move);
			UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Bound Move action"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[T66SimpleCharacter] MoveAction is NULL - cannot bind!"));
		}

		// Looking
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AT66SimpleCharacter::Look);
			UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Bound Look action"));
		}

		// Jumping
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AT66SimpleCharacter::StartJump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AT66SimpleCharacter::StopJump);
			UE_LOG(LogTemp, Warning, TEXT("[T66SimpleCharacter] Bound Jump action"));
		}
	}
}

void AT66SimpleCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// Get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AT66SimpleCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AT66SimpleCharacter::StartJump()
{
	Jump();
}

void AT66SimpleCharacter::StopJump()
{
	StopJumping();
}

void AT66SimpleCharacter::SetBodyColor(FLinearColor InColor)
{
	if (!BodyMaterial && BodyMesh)
	{
		BodyMaterial = UMaterialInstanceDynamic::Create(BodyMesh->GetMaterial(0), this);
		BodyMesh->SetMaterial(0, BodyMaterial);
	}
	
	if (BodyMaterial)
	{
		BodyMaterial->SetVectorParameterValue(FName(TEXT("BaseColor")), InColor);
	}
	
	UE_LOG(LogTemp, Display, TEXT("[T66SimpleCharacter] Set body color to R:%.2f G:%.2f B:%.2f"),
		InColor.R, InColor.G, InColor.B);
}

void AT66SimpleCharacter::ApplySelectedHeroAppearance()
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(GetWorld());
	if (!GI) return;
	
	UT66SelectionSubsystem* SelectionSS = GI->GetSubsystem<UT66SelectionSubsystem>();
	if (!SelectionSS) return;
	
	const FT66HeroData& SelectedHero = SelectionSS->GetSelectedHero();
	SetBodyColor(SelectedHero.BodyColor);
	
	UE_LOG(LogTemp, Display, TEXT("[T66SimpleCharacter] Applied hero appearance: %s"), *SelectedHero.DisplayName);
}

void AT66SimpleCharacter::SpawnCompanion()
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(GetWorld());
	if (!GI) return;
	
	UT66SelectionSubsystem* SelectionSS = GI->GetSubsystem<UT66SelectionSubsystem>();
	if (!SelectionSS) return;
	
	// Check if a companion is selected
	if (!SelectionSS->HasCompanionSelected())
	{
		UE_LOG(LogTemp, Display, TEXT("[T66SimpleCharacter] No companion selected"));
		return;
	}
	
	const FT66CompanionData& SelectedCompanion = SelectionSS->GetSelectedCompanion();
	
	// Spawn the companion actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	
	const FVector SpawnLocation = GetActorLocation() + FVector(-150.f, 100.f, 50.f);
	
	AT66CompanionActor* Companion = GetWorld()->SpawnActor<AT66CompanionActor>(
		AT66CompanionActor::StaticClass(),
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);
	
	if (Companion)
	{
		Companion->SetFollowTarget(this);
		Companion->SetCompanionColor(SelectedCompanion.BodyColor);
		SpawnedCompanion = Companion;
		
		UE_LOG(LogTemp, Display, TEXT("[T66SimpleCharacter] Spawned companion: %s"), *SelectedCompanion.DisplayName);
	}
}
