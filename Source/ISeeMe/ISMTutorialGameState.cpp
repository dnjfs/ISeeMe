// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMTutorialGameState.h"
#include "GameFramework/PlayerState.h"
#include <Net/UnrealNetwork.h>
#include "ISMTutorialController.h"

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
	// Enable Input (Plan)
	bInformation = false;

	if (OnTutorialUpdated.IsBound())
		OnTutorialUpdated.Execute(bInformation, TutorialStep); // Turn Practice State
}

void AISMTutorialGameState::MulticastInformation_Implementation()
{
	// Disable Input (Plan)
	bInformation = true;
	TutorialStep++;

	if (OnTutorialUpdated.IsBound())
		OnTutorialUpdated.Execute(bInformation, TutorialStep); // Send Next Information
}
