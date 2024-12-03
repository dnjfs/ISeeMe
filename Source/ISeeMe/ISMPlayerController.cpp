// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
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

void AISMPlayerController::ServerCallSwapCamera_Implementation()
{
	SwapCamera();
}

void AISMPlayerController::SwapCamera()
{
	if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
		GM->SwapCamera();
}

void AISMPlayerController::RecoverAspect_Implementation()
{
	ChangeUnHideBone(OtherCharacter);
}

void AISMPlayerController::SwapAspect()
{
	bFirstAspect = !bFirstAspect;  
	UpdateAspect();               
}

void AISMPlayerController::CurrentAspect()
{
	UpdateAspect(); 
}

void AISMPlayerController::UpdateAspect()
{
	if (bFirstAspect)
	{
		ChangeFirstAspect();
	}
	else
	{
		ChangeThirdAspect();
	}
}

void AISMPlayerController::ChangeThirdAspect()
{
	if (class USpringArmComponent* OtherCameraBoom = OtherCharacter->GetCameraBoom())
	{
		OtherCameraBoom->SetRelativeLocation(FVector(0,0,100));
		OtherCameraBoom->TargetArmLength = 600.0f;

		if (class UCameraComponent* OtherFollowCamera = OtherCharacter->GetFollowCamera())
			OtherFollowCamera->SetRelativeRotation(FRotator(-15, 0, 0));
	}
}

void AISMPlayerController::ChangeFirstAspect()
{
	if (class USpringArmComponent* OtherCameraBoom = OtherCharacter->GetCameraBoom())
	{
		AISeeMeCharacter* SelfCharacter = Cast<AISeeMeCharacter>(GetPawn());

		OtherCameraBoom->SetRelativeLocation(FVector(0, 0, 75));
		OtherCameraBoom->TargetArmLength = 0;

		if (class UCameraComponent* OtherFollowCamera = OtherCharacter->GetFollowCamera())
			OtherFollowCamera->SetRelativeRotation(FRotator(0, 0, 0));

		if (OtherCharacter && OtherCharacter->GetMesh() && SelfCharacter)
			if (OtherCharacter->GetMesh()->GetBoneIndex(SelfCharacter->HideBoneName) != INDEX_NONE)
				OtherCharacter->GetMesh()->HideBoneByName(SelfCharacter->HideBoneName, EPhysBodyOp::PBO_None);
	}
}

void AISMPlayerController::ChangeUnHideBone(AISeeMeCharacter* UnHideCharacter)
{
	AISeeMeCharacter* SelfCharacter = Cast<AISeeMeCharacter>(GetPawn());

	if (UnHideCharacter && UnHideCharacter->GetMesh() && bFirstAspect && SelfCharacter)
		if (UnHideCharacter->GetMesh()->GetBoneIndex(SelfCharacter->HideBoneName) != INDEX_NONE)
			UnHideCharacter->GetMesh()->UnHideBoneByName(SelfCharacter->HideBoneName);
}

