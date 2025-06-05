// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ISeeMeGameMode.h"
#include "ISeeMeCharacter.h"
#include "ISMCharacterState.h"
#include "ISMGameState.h"
#include "ISeeMe/UI/ISMHUD.h"
#include "ISeeMe/UI/ISMOverlay.h"
#include "Kismet/GameplayStatics.h"
#include "ISMLobbyController.h"
#include "ISMGameInstance.h"
#include "TimerManager.h"

AISMPlayerController::AISMPlayerController()
{
	bAutoManageActiveCameraTarget = false;
	bReplicates = true;
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

	if (IsLocalController())
	{
		if (ACharacter* LocalCharacter = Cast<ACharacter>(GetPawn()))
		{
			USkeletalMeshComponent* LocalMesh = LocalCharacter->GetMesh();
			if (LocalMesh)
			{
				LocalMesh->SetRenderCustomDepth(true);
			}
		}
	}

	if (HasAuthority())
	{
		if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
			GM->SwapCamera();
	}
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
	if (AISMGameState* GS = Cast<AISMGameState>(UGameplayStatics::GetGameState(this)))
	{
		if (GS->SwapViewItem == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot swap camera"));
			return; // 조건에 맞지 않으면 종료
		}

		if (HasAuthority()) // 서버에서 실행
		{
			if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
			{
				if (GM->SwapTimerHandle.IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("Already Swapped!"));
					return;
				}

				GetWorldTimerManager().SetTimer(GM->SwapTimerHandle, FTimerDelegate::CreateWeakLambda(this, [GM]()
					{
						GM->SwapCamera();
						GM->SwapTimerHandle.Invalidate();
					}), GM->SwapTime, false);
				GM->SwapCamera();

				GS->OnItemUsed(GS->SwapViewItem);
			}
		}
	}
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
		OtherCameraBoom->SetRelativeLocation(FVector(0, 0, 0));
		OtherCameraBoom->TargetArmLength = 600.0f;
	}
}

void AISMPlayerController::ChangeFirstAspect()
{
	if (OtherCharacter && OtherCharacter->GetMesh())
		if (class USpringArmComponent* OtherCameraBoom = OtherCharacter->GetCameraBoom())
		{
			OtherCameraBoom->SetRelativeLocation(OtherCharacter->FirstAspectLocation);
			OtherCameraBoom->TargetArmLength = 0;
		}
}

void AISMPlayerController::DeadCharacter()
{
	AISMCharacterState* State = GetPlayerState<AISMCharacterState>();
	if (State == nullptr)
		return;

	ACharacter* MyCharacter = GetCharacter();
	if (MyCharacter == nullptr)
		return;

	if (USceneComponent* RespawnPoint = State->GetRespawnPoint(State->CustomPlayerID)) // When get check point
	{
		MyCharacter->SetActorLocation(RespawnPoint->GetComponentLocation());
		MyCharacter->SetActorRotation(RespawnPoint->GetComponentRotation());
	}
	else // When don't get check point
	{
		MyCharacter->SetActorLocation(State->InitSpawnPointLocation);
		MyCharacter->SetActorRotation(State->InitialSpawnPointRotator);
	}
}