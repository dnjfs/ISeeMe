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
	SetControlRotation(FRotator(-20.f, 0.f, 0.f));

	/*if (IsLocalController())
	{
		AHUD* HUD = GetHUD();
		if (AISMHUD* PCHUD = Cast<AISMHUD>(HUD))
		{
			UE_LOG(LogTemp, Warning, TEXT("GET HUD"));
			PCHUD->InitWidgets();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NO HUD"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT LocalController"));
	}*/
}

void AISMPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (HasAuthority())
	{
		if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
			GM->SwapCamera();
	}

	UE_LOG(LogTemp, Warning, TEXT("OnPossess Called. IsLocalController: %s"), *FString(IsLocalController() ? "true" : "false"));
	
	/*if (IsLocalController())
	{
		AHUD* HUD = GetHUD();
		if (AISMHUD* PCHUD = Cast<AISMHUD>(HUD))
		{
			UE_LOG(LogTemp, Warning, TEXT("GET HUD"));
			PCHUD->InitWidgets();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NO HUD"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT LocalController"));
	}*/
}

void AISMPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AISMPlayerController, OtherCharacter);
}

void AISMPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (!IsLocalController())
	{
		return;
	}

	// 내 캐릭터 강조를 위해 '커스텀 뎁스 패스 렌더(bRenderCustomDepth)' 켜기
	if (GetCharacter())
	{
		if (USkeletalMeshComponent* LocalMesh = GetCharacter()->GetMesh())
		{
			LocalMesh->SetRenderCustomDepth(true);
		}
	}
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

void AISMPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsLocalController())
	{
		AHUD* HUD = GetHUD();
		if (AISMHUD* PCHUD = Cast<AISMHUD>(HUD))
		{
			UE_LOG(LogTemp, Warning, TEXT("GET HUD"));
			PCHUD->RemoveWidgets();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NO HUD"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT LocalController"));
	}

	Super::EndPlay(EndPlayReason);
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

void AISMPlayerController::ClientGoToLevel_Implementation()
{
	UGameplayStatics::OpenLevel(this, "LoadingMap");
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

void AISMPlayerController::CallReadTutorial()
{
	if (!bDoneRead)
	{
		bDoneRead = true;
		if (HasAuthority())
		{
			ReadTutorial();
		}
		else
		{
			ServerReadTutorial();
		}
	}
}

void AISMPlayerController::ReadTutorial()
{
	if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->CountReadEnding();
	}
}

void AISMPlayerController::ServerReadTutorial_Implementation()
{
	ReadTutorial();
}