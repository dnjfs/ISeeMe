// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMPlayerController.h"
#include "Components/AudioComponent.h"
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
#include "Kismet/GameplayStatics.h"

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

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ViewPitchMin = -70.f;
		PlayerCameraManager->ViewPitchMax = 40.f;
	}
}

void AISMPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	check(GetWorld());

	AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode());
	if (!IsValid(GM))
	{
		return;
	}

	TSubclassOf<AISeeMeCharacter> DesiredClass;
	if (UISMGameInstance* GameInstance = GetGameInstance<UISMGameInstance>())
	{
		if (IsLocalController())
		{
			DesiredClass = GameInstance->SelectedPawnClass;
		}
		else
		{
			DesiredClass = GameInstance->ClientPawnClass;
		}
	}
	
	// if statement with initializer (C++17)
	if (UClass* PossessedClass = aPawn->GetClass(); DesiredClass && PossessedClass != DesiredClass)
	{
		if (GM->ChangePawn(this, DesiredClass))
		{
			// 정상적으로 새로운 캐릭터를 스폰한 경우 OnPossess()가 한 번 더 호출됨
			return;
		}
	}

	// [NOTE] 레이스 컨디션 완화를 위해 1프레임 딜레이 발생
	// ChangePawn() 함수 내에서 스폰된 캐릭터의 정보가 클라이언트에 아직 반영되지 않은 상태에서
	// SwapCamera() 함수 내에서 SetViewTarget()의 인자로 사용되면 리플리케이션 시 nullptr로 받을 수 있음
	TWeakObjectPtr<AISeeMeGameMode> WeakWorld(GM);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimerForNextTick([WeakWorld]() {
		if (TStrongObjectPtr<AISeeMeGameMode> StrongWorld = WeakWorld.Pin(false))
		{
			StrongWorld->SwapCamera();
		}
	});
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

				TWeakObjectPtr<AISeeMeGameMode> WeakGameMode(GM);
				GetWorldTimerManager().SetTimer(GM->SwapTimerHandle, FTimerDelegate::CreateWeakLambda(this, [WeakGameMode]()
					{
						if (TStrongObjectPtr<AISeeMeGameMode> StrongGameMode = WeakGameMode.Pin(false))
						{
							StrongGameMode->SwapCamera();
							StrongGameMode->SwapTimerHandle.Invalidate();
						}
					}), GM->SwapTime, false);
				GM->RestoreCamera();

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

void AISMPlayerController::ClientPlayLocalSound_Implementation(USoundBase* SoundToPlay, bool bPlay)
{
	if (bPlay)
	{
		if (AISeeMeCharacter* PlayerCharacter = Cast<AISeeMeCharacter>(GetCharacter()))
		{
			if (SoundToPlay && PlayerCharacter->GimmickAudioComponent)
			{
				PlayerCharacter->GimmickAudioComponent->SetSound(SoundToPlay);
				PlayerCharacter->GimmickAudioComponent->Play();
			}
		}
	}
	else
	{
		if (AISeeMeCharacter* PlayerCharacter = Cast<AISeeMeCharacter>(GetCharacter()))
		{
			if (SoundToPlay && PlayerCharacter->GimmickAudioComponent && PlayerCharacter->GimmickAudioComponent->IsPlaying())
			{
				PlayerCharacter->GimmickAudioComponent->FadeOut(1.0f, 0.0f);
			}
		}
	}
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

	int RespawnIndex = IsLocalController() ? 1 : 2;
	if (USceneComponent* RespawnPoint = State->GetRespawnPoint(RespawnIndex)) // When get check point
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