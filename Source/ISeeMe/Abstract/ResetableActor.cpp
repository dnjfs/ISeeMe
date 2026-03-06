// Fill out your copyright notice in the Description page of Project Settings.


#include "ResetableActor.h"
#include "ISeeMe/ISeeMeGameMode.h"

AResetableActor::AResetableActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AResetableActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnCharacterReturned.AddDynamic(this, &AResetableActor::OnReset);
	}
}
