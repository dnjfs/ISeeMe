// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/SpringArmComponent.h"
#include "ISeeMeGameMode.h"
#include "ISeeMeCharacter.h"


AISMPlayerController::AISMPlayerController()
{
	bAutoManageActiveCameraTarget = false;
}

void AISMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}

void AISMPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (HasAuthority())
		SwapCamera();
}

void AISMPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AISMPlayerController, OtherCharacter);
}

void AISMPlayerController::OnRep_SwapCamera()
{
	if (OtherCharacter == nullptr)
		return;

	SetViewTarget(OtherCharacter);
	CurrentAspect();

	UE_LOG(LogTemp, Warning, TEXT("Camera Swap Success!"));
}

void AISMPlayerController::SwapCamera()
{
	if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->SwapCamera();
	}
}

void AISMPlayerController::SwapAspect()
{
	if (bFirstAspect)
	{
		if (class USpringArmComponent* OtherCameraBoom = OtherCharacter->GetCameraBoom())
		{
			OtherCameraBoom->TargetArmLength = 400.0f;
		}
	}
	else
	{
		if (class USpringArmComponent* OtherCameraBoom = OtherCharacter->GetCameraBoom())
		{
			OtherCameraBoom->TargetArmLength = 0;
			if (OtherCharacter && OtherCharacter->GetMesh())
			{
				OtherCharacter->GetMesh()->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
			}
		}
	}
	bFirstAspect = !bFirstAspect;
}

void AISMPlayerController::CurrentAspect()
{
	if (!bFirstAspect)
	{
		if (class USpringArmComponent* OtherCameraBoom = OtherCharacter->GetCameraBoom())
		{
			OtherCameraBoom->TargetArmLength = 400.0f;
		}
	}
	else if (bFirstAspect)
	{
		if (class USpringArmComponent* OtherCameraBoom = OtherCharacter->GetCameraBoom())
		{
			OtherCameraBoom->TargetArmLength = 0;
			if (OtherCharacter && OtherCharacter->GetMesh())
			{
				OtherCharacter->GetMesh()->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
			}
		}
	}
}

void AISMPlayerController::ServerCallSwapCamera_Implementation()
{
	SwapCamera();
}

void AISMPlayerController::RecoverAspect_Implementation()
{
	if (OtherCharacter)
		OtherCharacter->GetMesh()->UnHideBoneByName(FName("neck_01"));
}
