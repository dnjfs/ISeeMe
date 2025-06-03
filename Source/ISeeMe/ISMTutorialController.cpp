// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMTutorialController.h"
#include "ISMTutorialGameState.h"
#include <Kismet/GameplayStatics.h>

void AISMTutorialController::ServerCallSwapCamera()
{
	Super::ServerCallSwapCamera();

	if (AISMTutorialGameState* GS = Cast<AISMTutorialGameState>(UGameplayStatics::GetGameState(this)))
	{
		UseItemPractice();
	}
}

void AISMTutorialController::BeginPlay()
{
	Super::BeginPlay();
}

void AISMTutorialController::UseItemPractice()
{
	if (AISMTutorialGameState* GS = Cast<AISMTutorialGameState>(UGameplayStatics::GetGameState(this)))
	{
		GS->MulticastInformation();
	}
}
