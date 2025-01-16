// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMGameState.h"
#include <Net/UnrealNetwork.h>
#include "ISMGameInstance.h"
#include "ISeeMeCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "ISMPlayerController.h"

AISMGameState::AISMGameState()
{
	bReplicates = true;
	SwapViewItem = nullptr;
	SaveSwapViewItem = nullptr;
	UsedSwapViewItems.Empty();
	bAcqCheckPoint = false;
}

void AISMGameState::BeginPlay()
{
	if (UISMGameInstance* GameInstance = GetGameInstance<UISMGameInstance>())
	{
		if (GameInstance->SelectedPawnClass)
		{
			AddSelectedPawnClass(GameInstance->SelectedPawnClass);
			UE_LOG(LogTemp, Warning, TEXT("Go %s"), *GameInstance->SelectedPawnClass->GetName());
			if (HasAuthority())
			{
				for (int i = 0; i < SelectedPawnClasses.Num(); i++)
				{
					UE_LOG(LogTemp, Warning, TEXT("Server : Goes %s"), *SelectedPawnClasses[i]->GetName());
				}
			}
			else
			{
				for (int i = 0; i < SelectedPawnClasses.Num(); i++)
				{
					UE_LOG(LogTemp, Warning, TEXT("Client : Goes %s"), *SelectedPawnClasses[i]->GetName());
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No GameInstance->SelectedPawnClass in GameState"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No GameInstance in GameState"));
	}
}

void AISMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AISMGameState, SwapViewItem);
	DOREPLIFETIME(AISMGameState, SaveSwapViewItem);
	DOREPLIFETIME(AISMGameState, UsedSwapViewItems);
	DOREPLIFETIME(AISMGameState, SelectedPawnClasses);
}

void AISMGameState::AddSelectedPawnClass(const TSubclassOf<APawn>& NewPawn)
{
	if (HasAuthority())
		SelectedPawnClasses.Add(NewPawn);
}

