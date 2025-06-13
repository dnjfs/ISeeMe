// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMTutorialController.h"
#include "ISMTutorialGameState.h"
#include <Kismet/GameplayStatics.h>
#include "ISeeMeGameMode.h"

void AISMTutorialController::SetPawn(APawn* aPawn)
{
	Super::SetPawn(aPawn);

	// 클라이언트가 늦게 접속되었을 때 서버까지 입력이 되지 않는 문제가 발생함
	// 분명 각자 처리하는데 왜 영항을 주는지 모르겠음..
	// 결국 PossessedBy() / OnRep_Controller() 에서 서버 클라 구분해서 처리
	//if (IsLocalController() && aPawn)
	//{
	//	aPawn->DisableInput(this);
	//}
}

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
}

