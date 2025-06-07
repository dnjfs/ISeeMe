// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMAngel.h"
#include "ISMTutorialGameState.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AISMAngel::AISMAngel()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));

	if(SpawnPoint)
		SpawnPoint->SetupAttachment(RootComponent);

	bTutorialSpawn.Add(false);
	bTutorialSpawn.Add(false);
}

// Called when the game starts or when spawned
void AISMAngel::BeginPlay()
{
	Super::BeginPlay();
	
	Move(true);
}

void AISMAngel::Move(bool bInformation)
{
	if (AISMTutorialGameState* GS = Cast<AISMTutorialGameState>(UGameplayStatics::GetGameState(this)) )
	{
		if(GS->TutorialStep>=0 && GS->TutorialStep< InformationLocation.Num())
		{
			if(bInformation)
				SetActorLocation(InformationLocation[GS->TutorialStep]);
			else
			{
				SetActorLocation(PracticeLocation[GS->TutorialStep]);
				return;
			}
		}

		if (!HasAuthority())
			return;

		FVector SpawnLoc = SpawnPoint->GetComponentLocation();
		FRotator SpawnRot = SpawnPoint->GetComponentRotation();
		if (GS->TutorialStep == 1 && !bTutorialSpawn[0])
		{
			GetWorld()->SpawnActor<AActor>(TutorialItems[0], SpawnLoc, SpawnRot);
			bTutorialSpawn[0] = true;
		}
		else if (GS->TutorialStep == 3 && !bTutorialSpawn[1])
		{
			GetWorld()->SpawnActor<AActor>(TutorialItems[1], SpawnLoc, SpawnRot);
			bTutorialSpawn[1] = true;
		}
	}
}

