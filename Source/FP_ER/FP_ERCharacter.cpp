// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FP_ERCharacter.h"
#include "FP_ERProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFP_ERCharacter

AFP_ERCharacter::AFP_ERCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
}

void AFP_ERCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Can Turn: %s"), bCanTurn ? TEXT("True") : TEXT("False")));
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Is Dead: %s"), bIsDead ? TEXT("True") : TEXT("False")));
	}
	AutoRun(DeltaTime);
}

void AFP_ERCharacter::AutoRun(float DeltaTime)
{
	//Don't run if dead
	if (bIsDead)return;
	// Check for turn
	CheckforTurn();
	//Sanity check
	if (Controller != NULL)
	{		
		// add movement in that direction
		AddMovementInput(GetActorForwardVector());
		printf("Moving");
	}
}

void AFP_ERCharacter::CheckforTurn()
{
	//Get rotation every frame
	FRotator InitialRotation = GetControlRotation();
	//debug message
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Red, FString::Printf(TEXT("Initial Rot: %s , Desired Rot: %s"), *InitialRotation.ToString(), *DesiredRotation.ToString()));
	}
	//if current rotation is not equivalent to desired rotation then make it so
	if (InitialRotation != DesiredRotation)
	{
		// Smoothly turn
		FRotator NewRotation = FMath::RInterpTo(InitialRotation, DesiredRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
		// Apply turn
		Controller->SetControlRotation(NewRotation);
		printf("Rotated");
	}
}

void AFP_ERCharacter::AddCoin()
{
	printf("Coin Added");
}

void AFP_ERCharacter::Death()
{
	// If alive stop
	if (!bIsDead)return;
	printf("Dead");
}

void AFP_ERCharacter::RotateAtTurn(float Value)
{
	// Sanity check + Pre-Requisite is met - bcanTurn
	if ((Controller != NULL && Value != 0.0f) && bCanTurn)
	{
		// If axis is positive turn right
		if (Value > 0.0f)
		{
			//Positive --- 0
			DesiredRotation += FRotator(0.0f, 90.0f, 0.0f);
			bCanTurn = false;
		}
		// If axis is positive turn left
		else
		{
			DesiredRotation += FRotator(0.0f, -90.0f, 0.0f);
			bCanTurn = false;
		}
	}
}

void AFP_ERCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	Mesh1P->SetHiddenInGame(false, true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFP_ERCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFP_ERCharacter::Raycast);

	PlayerInputComponent->BindAxis("Rotate", this, &AFP_ERCharacter::RotateAtTurn);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFP_ERCharacter::MoveRight);
}

void AFP_ERCharacter::Raycast()
{
	printf("Raycast");
	//Get ref to world
	UWorld* const World = GetWorld();
	// Sanity check
	if (World)
	{
		// Get ref to controller
		APlayerController* MyController = World->GetFirstPlayerController();
		// Sanity check
		if (MyController != nullptr)
		{
			// Get mouse co-ordinates
			float LocationX;
			float LocationY;
			MyController->GetMousePosition(LocationX, LocationY);

			//perform trace
			FVector2D MousePosition(LocationX, LocationY);
			FHitResult HitResult;
			const bool bTraceComplex = false;
			if (MyController->GetHitResultAtScreenPosition(MousePosition, ECC_Visibility, bTraceComplex, HitResult))
			{
				printf("HitSomething");
				HitResult.GetActor()->Destroy();
			}
		}
	}
		
	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AFP_ERCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			
			// spawn the projectile at the muzzle
			World->SpawnActor<AFP_ERProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AFP_ERCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}
