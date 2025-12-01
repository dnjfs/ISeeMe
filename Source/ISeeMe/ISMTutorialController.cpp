// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMTutorialController.h"
#include "ISMTutorialGameState.h"
#include <Kismet/GameplayStatics.h>
#include "ISeeMeGameMode.h"

void AISMTutorialController::SetPawn(APawn* aPawn)
{
	Super::SetPawn(aPawn);

	// クライアントが遅れて接続されたとき、サーバーまで入力が届かない問題が発生する
	// 明らかにそれぞれ処理しているのに、なぜ影響を与えるのか分からない…
	// 結局 PossessedBy() / OnRep_Controller() でサーバーとクライアントを区別して処理
	//if (IsLocalController() && aPawn)
	//{
	//	aPawn->DisableInput(this);
	//}
}

void AISMTutorialController::SwapCamera()
{
	if (AISMTutorialGameState* GS = Cast<AISMTutorialGameState>(UGameplayStatics::GetGameState(this)))
	{
		if (!GS->bInformation && GS->TutorialStep == 2)
		{
			Super::SwapCamera();
			GS->MulticastInformation();
		}
	}
}

