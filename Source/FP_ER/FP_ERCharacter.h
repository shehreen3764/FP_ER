// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FP_ERCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AFP_ERCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;


public:
	AFP_ERCharacter();

protected:
	virtual void BeginPlay();

public:
	virtual void Tick(float DeltaTime) override;

	void AutoRun(float DeltaTime);
	
	void CheckforTurn();

	void AddCoin();

	void Death();

	void RotateAtTurn(float Value);

	void Raycast();

	/*Ray Length*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Niall")
		float RayLength = 5000.0f;

	/*Desired rotation*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Niall")
	FRotator DesiredRotation;

	/*Rotation speed*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Niall")
	float RotationSpeed = 10.0f;

	/*Turn Zone Bool*/
	UPROPERTY(EditAnywhere, Category = "Custom Niall")
	uint32 bCanTurn : 1;

	/*Is Dead Bool*/
	UPROPERTY(EditAnywhere, Category = "Custom Niall")
	uint32 bIsDead : 1;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AFP_ERProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

