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

	if (bSpawn)
	{
		if (HasAuthority())
		{
			bSpawn = false;
		}
		else
		{
			ServerSpawnClass();
		}
	}

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}

void AISMPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (aPawn && bPossess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Possessing Pawn: %s"), *aPawn->GetName());

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(
			TimerHandle,
			this,
			&AISMPlayerController::OneSpawnClass,
			1.0f, // 딜레이 시간
			false // 반복 여부
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnPossess called with null Pawn or bPossess is false."));
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

void AISMPlayerController::ServerSpawnClass_Implementation()
{
	/*if (AISMGameState* GS = Cast<AISMGameState>(UGameplayStatics::GetGameState(this)))
		if(UISMGameInstance* GameInstance = GetGameInstance<UISMGameInstance>())
			if(GameInstance->SelectedPawnClass)
				GS->AddSelectedPawnClass(GameInstance->SelectedPawnClass);*/

	if (AISMGameState* GS = Cast<AISMGameState>(UGameplayStatics::GetGameState(this)))
	{
		if (UISMGameInstance* GameInstance = GetGameInstance<UISMGameInstance>())
		{
			if (GameInstance->SelectedPawnClass)
				GS->AddSelectedPawnClass(GameInstance->SelectedPawnClass);
			else
				UE_LOG(LogTemp, Warning, TEXT("No GameInstance->SelectedPawnClass in PlayerController"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No GameInstance in PlayerController"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No GameState in PlayerController"));
	}
}

void AISMPlayerController::SpawnClass()
{
	TArray<AISeeMeCharacter*> LocalCharacters;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		ACharacter* BaseCharacter = PC->GetCharacter();
		if (BaseCharacter && BaseCharacter->IsA(AISeeMeCharacter::StaticClass()))
		{
			AISeeMeCharacter* LocalCharacter = Cast<AISeeMeCharacter>(BaseCharacter);
			LocalCharacters.Add(LocalCharacter);
		}
	}

	AISMGameState* GS = Cast<AISMGameState>(UGameplayStatics::GetGameState(this));
	if (GS==nullptr)
	{
		return;
	}

	if (LocalCharacters.Num() != GS->SelectedPawnClasses.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Mismatch between Characters (%d) and SelectedPawnClasses (%d) count!"),
			LocalCharacters.Num(), GS->SelectedPawnClasses.Num());
		return;
	}

	for (int i = 0; i < LocalCharacters.Num(); i++)
	{
		AISeeMeCharacter* MyCharacter = LocalCharacters[i];
		if (GS->SelectedPawnClasses[i])
		{
			APawn* ExistingPawn = MyCharacter->GetController()->GetPawn();
			AISMPlayerController* Controller = Cast<AISMPlayerController>(MyCharacter->GetController());
			Controller->bPossess = false;
			if (ExistingPawn)
			{
				UE_LOG(LogTemp, Warning, TEXT("Destroying existing pawn for player %d: %s"), i, *ExistingPawn->GetName());
				ExistingPawn->Destroy();
			}

			FVector MySpawnLocation = MyCharacter->GetActorLocation();
			FRotator MySpawnRotation = MyCharacter->GetActorRotation();
			if (!MySpawnLocation.ContainsNaN() && !MySpawnRotation.ContainsNaN())
			{
				APawn* NewPawn = GetWorld()->SpawnActor<APawn>(GS->SelectedPawnClasses[i], MySpawnLocation, MySpawnRotation);

				if (NewPawn)
				{
					if (Controller && NewPawn)
					{
						Controller->Possess(NewPawn);
						UE_LOG(LogTemp, Warning, TEXT("Spawned and possessed new pawn for player %d"), i);
						UE_LOG(LogTemp, Warning, TEXT("Pawn Name (%s) and Num (%d) Controller"),
							*NewPawn->GetName(), i, *GetName());
					}
				}
			}
		}
	}
}

void AISMPlayerController::OneSpawnClass()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnPosses"));
		SpawnClass();
		SwapCamera(false);
	}
}

void AISMPlayerController::ServerCallSwapCamera_Implementation()
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

				UE_LOG(LogTemp, Warning, TEXT("Use"));

				// 아이템 소지 UI 비활성화
				GS->SwapViewItem->MulticastSetOverlayIcon(false);

				GS->UsedSwapViewItems.Add(GS->SwapViewItem);
				GS->SwapViewItem = nullptr;
			}
		}
	}
}

void AISMPlayerController::SwapCamera(bool bItem)
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
		OtherCameraBoom->SetRelativeLocation(FVector(0,0,0));
		OtherCameraBoom->TargetArmLength = 600.0f;
	}
}

void AISMPlayerController::ChangeFirstAspect()
{
	if (class USpringArmComponent* OtherCameraBoom = OtherCharacter->GetCameraBoom())
	{
		AISeeMeCharacter* SelfCharacter = Cast<AISeeMeCharacter>(GetPawn());

		OtherCameraBoom->SetRelativeLocation(FVector(0, 0, 75));
		OtherCameraBoom->TargetArmLength = 0;

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
