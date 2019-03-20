#include "BaseTile.h"
#include "TileGenerator.h"
#include "FP_ERCharacter.h" 

// Sets default values
ABaseTile::ABaseTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		for (TActorIterator<ATileGenerator> ActorItr(World); ++ActorItr)
		{
			TileManager = *ActorItr;
		}
	}
}

//this is to check whether git bash is working or not

void ABaseTile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitializeArrows();
	InitializeTriggers();
	Triggers[0]->OnComponentBeginOverlap.AddDynamic(this, &ABaseTile::OnTriggerEnter);
}

// Called when the game starts or when spawned
void ABaseTile::BeginPlay()
{
	Super::BeginPlay();
	uint8 Result = FMath::RandHelper(2);
	switch (Result)
	{
	case 0:
		CreateCoins();
		break;
	case 1:
		CreateObstacles();
		break;
	}
}

// Called every frame
void ABaseTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseTile::InitializeTriggers()
{
	TArray<UActorComponent*> Components = GetComponentByClass(UBoxComponent::StaticClass());
	if (Components.Num() == 0) return;
	for (size_t i = 0; i < Components.Num(); i++)
	{
		UBoxComponent* ThisTrigger = Cast<UBoxComponent>(Components[i]);
		if (ThisTrigger != nullptr)
		{
			//0 spawner 1 coin box
			ThisTrigger->bGenerateOverlapEvents = true;
			Triggers.Add(ThisTrigger);
		}
	}
}


void ABaseTile::InitializeArrows()
{
	TArray<UActorComponent*> Components = GetComponentByClass(UArrowComponent::StaticClass());
	if (Components.Num() == 0) return;
	for (size_t i = 0; i < Components.Num(); i++)
	{
		UArrowComponent* ThisArrow = Cast<UArrowComponent>(Components[i]);
		if (ThisArrow != nullptr)
		{
			Arrows.Add(ThisArrow);
		}
	}
}

void ABaseTile::CreateObstacles()
{

}

void ABaseTile::CreateCoins()
{

}

FTransform ABaseTile::SetObstacleSpawns()
{
	uint8 ObstacleSpawnIndex = FMath::RandHelper(3);

	if (Arrows[ObstacleSpawnIndex + 1] != nullptr)
	{
		return Arrows[ObstacleSpawnIndex + 1]->GetComponentTransform();
	}
	else
	{
		return FTransform();
	}

}

FTransform ABaseTile::GetNextSpawnTransform()
{
	if (Arrows[0] != nullptr)
	{
		return Arrows[0]->GetComponentTransform();
	}
	else
	{
		return FTransform();
	}

}

void ABaseTile::OnTriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepREsult)
{
	//check against player
	AFP_ERCharacter* MyCharacter = Cast< AFP_ERCharacter>(OtherActor);
	if (MyCharacter != nullptr)
	{
		//get ref to world
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (TileManager != nullptr)
			{
				//make 1 tile
				TileManager->CreateTile();
				//delete after 2 sec
				World->GetTimerManager().SetTimer(DelayForDestroyMe, this, &ABaseTile::DestroyMe, 2.0f);
			}
		}
	}
}

void ABaseTile::DestroyMe()
{
	Destroy();
}