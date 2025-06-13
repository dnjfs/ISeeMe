// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMTutorialGameState.h"
#include "ISMTutorialStepDoneActor.h"
#include "GameFramework/PlayerState.h"
#include <Net/UnrealNetwork.h>
#include "Kismet/GameplayStatics.h"
#include "ISMTutorialController.h"
#include "ISMAngel.h"

void AISMTutorialGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AISMTutorialGameState, bInformation);
	DOREPLIFETIME(AISMTutorialGameState, TutorialStep);
}

AISMTutorialGameState::AISMTutorialGameState()
{
	TutorialStep = 0;
}

void AISMTutorialGameState::Practice()
{
	// Enable Input
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		if (APawn* Pawn = PC->GetPawn())
		{
			Pawn->EnableInput(PC);
		}
	}

	bInformation = false;
	SendTutorialState();

	if (OnTutorialUpdated.IsBound())
		OnTutorialUpdated.Execute(bInformation, TutorialStep); // Turn Practice State
}

void AISMTutorialGameState::MulticastInformation_Implementation()
{
	// Disable Input
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		if (APawn* Pawn = PC->GetPawn())
		{
			Pawn->DisableInput(PC);
		}
	}

	bInformation = true;
	TutorialStep++;
	SendTutorialState();

	if (OnTutorialUpdated.IsBound())
		OnTutorialUpdated.Execute(bInformation, TutorialStep); // Send Next Information
}

void AISMTutorialGameState::SendTutorialState()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AISMTutorialStepDoneActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AISMTutorialStepDoneActor* TriggerActor = Cast<AISMTutorialStepDoneActor>(Actor);
		if (TriggerActor)
		{
			TriggerActor->ChangeCollisionState();
		}
	}


	AISMAngel* Angel = Cast<AISMAngel>(UGameplayStatics::GetActorOfClass(GetWorld(), AISMAngel::StaticClass()));

	if (Angel)
	{
		Angel->Move(bInformation);
	}
}
