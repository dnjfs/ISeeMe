// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMTutorialController.h"
#include "ISMTutorialGameState.h"
#include <Kismet/GameplayStatics.h>
#include "ISeeMeGameMode.h"

void AISMTutorialController::SwapCamera()
{
	if (AISMTutorialGameState* GS = Cast<AISMTutorialGameState>(UGameplayStatics::GetGameState(this)))
	{
		if(!GS->bInformation && GS->TutorialStep==2)
		{
			Super::SwapCamera(); 
			GS->MulticastInformation();
		}
	}
	else
	{
		Super::SwapCamera();
	}
}

