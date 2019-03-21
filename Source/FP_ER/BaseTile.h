// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include  "FP_ER.h"
#include "TileGenerator.h"
#include "BaseTile.generated.h"

UCLASS()
class FP_ER_API ABaseTile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties


	ABaseTile();


	/**
	 * The functions of interest to initialization order for an Actor is roughly as follows:
	 * PostLoad/PostActorCreated - Do any setup of the actor required for construction. PostLoad for serialized actors, PostActorCreated for spawned.
	 * AActor::OnConstruction - The construction of the actor, this is where Blueprint actors have their components created and blueprint variables are initialized
	 * AActor::PreInitializeComponents - Called before InitializeComponent is called on the actor's components
	 * UActorComponent::InitializeComponent - Each component in the actor's components array gets an initialize call (if bWantsInitializeComponent is true for that component)
	 * AActor::PostInitializeComponents - Called after the actor's components have been initialized
	 * AActor::BeginPlay - Called when the level is started
	 */
	virtual void PostInitializeComponents() override;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializeArrows();
	void InitializeTriggers();
	void CreateObstacles();
	void CreateCoins();

	FTransform SetObstacleSpawns();

	UFUNCTION()
		FTransform GetNextSpawnTransform();

	UFUNCTION()
		void OnTriggerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void DestroyMe();

public:
	TArray<UArrowComponent*> Arrows;
	TArray<UBoxComponent*> Triggers;
	FTimerHandle DelayForDestroyMe;
	class ATileGenerator* TileManager;


};