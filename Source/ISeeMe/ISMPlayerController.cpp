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

	if (HasAuthority())
	{
		SwapCamera(false);
	}
	/*if (bSpawn)
	{
		if (HasAuthority())
		{
			UE_LOG(LogTemp, Warning, TEXT("Server"));
			SpawnClass();
			SwapCamera(false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Client"));
			ServerSpawnClass();
			SpawnClass();
		}
	}*/
}

void AISMPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AISMPlayerController, OtherCharacter);
	DOREPLIFETIME(AISMPlayerController, bSpawn);
	DOREPLIFETIME(AISMPlayerController, SelectedPawnClasses);
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
	/*if (!HasAuthority()) // 서버에서 권한 확인
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerSpawnClass was called, but this is not the server!"));
		return;
	}
	SpawnClass();*/
	UISMGameInstance* GameInstance = GetGameInstance<UISMGameInstance>();
	if (GameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode : No GameInstance"));
		return;
	}
	else
	{
		SelectedPawnClasses.Add(GameInstance->SelectedPawnClass);
	}
}

void AISMPlayerController::SpawnClass()
{
	bSpawn = false;

	UISMGameInstance* GameInstance = GetGameInstance<UISMGameInstance>();
	if (GameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode : No GameInstance"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *GameInstance->SelectedPawnClass->GetName());
		SelectedPawnClasses.Add(GameInstance->SelectedPawnClass);
	}


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

	if (LocalCharacters.Num() != SelectedPawnClasses.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Mismatch between Characters (%d) and SelectedPawnClasses (%d) count!"),
			LocalCharacters.Num(), SelectedPawnClasses.Num());
		return;
	}

	for (int i = 0; i < LocalCharacters.Num(); i++)
	{
		AISeeMeCharacter* MyCharacter = LocalCharacters[i];
		if (TSubclassOf<APawn> SelectedClass = SelectedPawnClasses[i])
		{
			APawn* ExistingPawn = MyCharacter->GetController()->GetPawn();
			AController* Controller = MyCharacter->GetController();
			if (ExistingPawn)
			{
				UE_LOG(LogTemp, Warning, TEXT("Destroying existing pawn for player %d: %s"), i, *ExistingPawn->GetName());
				ExistingPawn->Destroy();
			}

			FVector MySpawnLocation = MyCharacter->GetActorLocation();
			FRotator MySpawnRotation = MyCharacter->GetActorRotation();
			if (!MySpawnLocation.ContainsNaN() && !MySpawnRotation.ContainsNaN())
			{
				APawn* NewPawn = GetWorld()->SpawnActor<APawn>(SelectedClass, MySpawnLocation, MySpawnRotation);

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
	UISMGameInstance* GameInstance = GetGameInstance<UISMGameInstance>();
	if (GameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode : No GameInstance"));
		return;
	}
	bSpawn = false;
	UE_LOG(LogTemp, Warning, TEXT("One Character"));
	AISeeMeCharacter* MyCharacter = Cast<AISeeMeCharacter>(GetCharacter());
	APawn* ExistingPawn = MyCharacter->GetController()->GetPawn();
	AController* Controller = MyCharacter->GetController();
	if (ExistingPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroying existing pawn for player 0: %s"), *ExistingPawn->GetName());
		ExistingPawn->Destroy();
	}

	FVector MySpawnLocation = MyCharacter->GetActorLocation();
	FRotator MySpawnRotation = MyCharacter->GetActorRotation();
	if (!MySpawnLocation.ContainsNaN() && !MySpawnRotation.ContainsNaN())
	{
		APawn* NewPawn = GetWorld()->SpawnActor<APawn>(GameInstance->SelectedPawnClass, MySpawnLocation, MySpawnRotation);

		if (NewPawn)
		{
			if (Controller && NewPawn)
			{
				Controller->Possess(NewPawn);
				UE_LOG(LogTemp, Warning, TEXT("Spawned and possessed new pawn for player 0"));
				UE_LOG(LogTemp, Warning, TEXT("Pawn Name (%s) and Num 0 Controller"),
					*NewPawn->GetName(), *GetName());
			}
		}
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
