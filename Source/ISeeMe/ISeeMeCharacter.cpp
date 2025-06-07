// Copyright Epic Games, Inc. All Rights Reserved.

#include "ISeeMeCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ISMPlayerController.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/GameMode.h"
#include "ISeeMeGameMode.h"
#include "ISeeMe/UI/ISMHUD.h"
#include "ISMCharacterState.h"
#include "Components/TimelineComponent.h"
#include "ISMGameState.h"
#include <Net/UnrealNetwork.h>
#include "Components/AudioComponent.h"
#include "ISMGameInstance.h"
#include "ISMTutorialController.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

#define	LOG_SCREEN(Format, ...) \
	if (GEngine)\
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

//////////////////////////////////////////////////////////////////////////
// AISeeMeCharacter

AISeeMeCharacter::AISeeMeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	FocusTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FocusTimeline"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AISeeMeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AISeeMeCharacter, IsCameraRestored);
}

void AISeeMeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// 내 캐릭터 강조를 위해 '커스텀 뎁스 패스 렌더(bRenderCustomDepth)' 켜기
	if (AController* LocalController = GetWorld()->GetFirstPlayerController())
	{
		APawn* LocalPawn = LocalController->GetPawn();
		if (ACharacter* LocalCharacter = Cast<ACharacter>(LocalPawn))
		{
			USkeletalMeshComponent* LocalMesh = LocalCharacter->GetMesh();
			if (LocalMesh)
			{
				LocalMesh->SetRenderCustomDepth(true);
			}
		}
	}

	// Save Transform 
	if (HasAuthority()) 
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
			{
				if (AISMCharacterState* State = Cast<AISMCharacterState>(GetPlayerState()))
				{
					State->InitSpawnPointLocation = GetActorLocation();
					State->InitialSpawnPointRotator = GetActorRotation();
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Fail: PlayerState is not initialized yet."));
				}

				// 호스트에 체크포인트가 저장되어있을 경우 작동
				if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
				{
					if (GI->SavedCheckPointID != FName("None"))
					{
						TArray<AActor*> CheckPoints;
						UGameplayStatics::GetAllActorsOfClass(GetWorld(), AISMCheckPoint::StaticClass(), CheckPoints);
						for (AActor* ACheckPoint : CheckPoints)
						{
							if (GI->SavedCheckPointID == ACheckPoint->GetFName()) // 체크포인트 찾음
							{
								AISMCheckPoint* ISMCheckPoint = Cast<AISMCheckPoint>(ACheckPoint);

								if (AISMGameState* GS = Cast<AISMGameState>(UGameplayStatics::GetGameState(this)))
								{
									if (GS->SwapViewItem != nullptr)
									{
										GS->bAcqCheckPoint = true;
										GS->SaveSwapViewItem = GS->SwapViewItem;
									}
									GS->UsedSwapViewItems.Empty();

									ISMCheckPoint->MulticastChangeMaterial(2);
									ISMCheckPoint->InitCheckPoint();

									if (AISMPlayerController* PC = GetController<AISMPlayerController>())
									{
										PC->DeadCharacter();
									}
								}
								break;
							}
							else
							{
								continue;
							}
						}
					}
				}
			});
	}


	if (FocusCurve)
	{
		FOnTimelineFloat OnTimelineFloat;
		OnTimelineFloat.BindUFunction(this, TEXT("PlayFocusTimeline"));

		FocusTimeline->AddInterpFloat(FocusCurve, OnTimelineFloat, TEXT("Alpha"));
		FocusTimeline->SetLooping(false);
		FocusTimeline->SetIgnoreTimeDilation(true);
	}
}


//////////////////////////////////////////////////////////////////////////
// Input

void AISeeMeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AISeeMeCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AISeeMeCharacter::Look);

		// Focus
		EnhancedInputComponent->BindAction(FocusAction, ETriggerEvent::Triggered, this, &AISeeMeCharacter::Focus);
	
		EnhancedInputComponent->BindAction(SwapCameraAction, ETriggerEvent::Started, this, &AISeeMeCharacter::SwapCamera);
		EnhancedInputComponent->BindAction(SwapAspectAction, ETriggerEvent::Started, this, &AISeeMeCharacter::SwapAspect);

		EnhancedInputComponent->BindAction(OpenMenuAction, ETriggerEvent::Triggered, this, &AISeeMeCharacter::OpenMenu);

		EnhancedInputComponent->BindAction(GoCheckPointAction, ETriggerEvent::Started, this, &AISeeMeCharacter::CallGoCheckPoint);

		EnhancedInputComponent->BindAction(ControlPitchAction, ETriggerEvent::Triggered, this, &AISeeMeCharacter::ControlPitch);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AISeeMeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	if (GetCharacterMovement()->IsFalling())
	{
		if (!bIsFalling)
		{
			FirstHeight = GetActorLocation().Z;
			bIsFalling = true;
		}
	}
	else
	{
		if (!bIsFalling)
			return;

		if (FirstHeight - GetActorLocation().Z >= DeadHeight)
		{
			if (AISMCharacterState* State = Cast<AISMCharacterState>(this->GetPlayerState()))
			{
				SetActorLocation(State->InitSpawnPointLocation);
				SetActorRotation(State->InitialSpawnPointRotator);
			}
		}
		bIsFalling = false;
	}
	*/
}

void AISeeMeCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AISeeMeCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);

		if (IsCameraRestored)
			AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AISeeMeCharacter::Focus()
{
	if (AISMPlayerController* ISMPlayerController = GetController<AISMPlayerController>())
	{
		if (ACharacter* OtherCharacter = ISMPlayerController->GetOtherCharacter())
		{
			FocusStartRotator = ISMPlayerController->GetControlRotation();

			float NewYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), OtherCharacter->GetActorLocation()).Yaw;
			FocusEndRotator = FRotator(FocusStartRotator.Pitch, NewYaw, FocusStartRotator.Roll);

			FocusTimeline->PlayFromStart();
		}
	}
}

void AISeeMeCharacter::PlayFocusTimeline(float Alpha)
{
	if (AController* MyController = GetController())
	{
		MyController->SetControlRotation(
			UKismetMathLibrary::RLerp(FocusStartRotator, FocusEndRotator, Alpha, true));
	}
}

void AISeeMeCharacter::OnRep_IsCameraRestored()
{
	if (IsCameraRestored)
	{
		CameraBoom->bInheritPitch = true;
	}
	else
	{
		CameraBoom->bInheritPitch = false;
	}
}

void AISeeMeCharacter::SwapCamera()
{
	if (AISMPlayerController* PC = Cast<AISMPlayerController>(GetController()))
	{
		PC->ServerCallSwapCamera();
	}
}

void AISeeMeCharacter::SwapAspect()
{
	// Swap Aspect
	if (AISMPlayerController* PC = Cast<AISMPlayerController>(GetController()))
		if (PC->OtherCharacter)
		{
			PC->SwapAspect();
		}
}

void AISeeMeCharacter::CallGoCheckPoint()
{
	if (HasAuthority()) // Call Go Check Point Function on the server
	{
		GoCheckPoint();
	} 
	else // Call Go Check Point Function on the client
	{
		ServerCallGoCheckPoint();
	}
}

void AISeeMeCharacter::ControlPitch(const FInputActionValue& Value)
{
	if (IsCameraRestored)
		return;

	const float FloatValue = Value.Get<float>();

	if (AISMPlayerController* ISMPlayerController = GetController<AISMPlayerController>())
	{
		if (AISeeMeCharacter* OtherCharacter = ISMPlayerController->GetOtherCharacter())
		{
			if (OtherCharacter->GetCameraBoom()->bInheritPitch == true)
				OtherCharacter->GetCameraBoom()->bInheritPitch = false;

			if (FloatValue > 0)
				OtherCharacter->GetCameraBoom()->AddRelativeRotation(FRotator(1.f, 0.f, 0.f));
			else
				OtherCharacter->GetCameraBoom()->AddRelativeRotation(FRotator(-1.f, 0.f, 0.f));
		}
	}
}

void AISeeMeCharacter::GoCheckPoint()
{
	// Return Swap Camera State
	if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (!GM->bSwapCamera)
		{
			GetWorldTimerManager().ClearTimer(GM->SwapTimerHandle);
			GM->SwapCamera();
		}
	}

	MulticastPlaySound();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get()))
			PC->DeadCharacter();
	}

	if (AISMGameState* GS = Cast<AISMGameState>(UGameplayStatics::GetGameState(this)))
	{
		GS->MulticastReturnSwapViewItem();
	}
}

void AISeeMeCharacter::ServerCallGoCheckPoint_Implementation()
{
	GoCheckPoint(); //  Call Go Check Point Function from client to the server
}

void AISeeMeCharacter::MulticastPlaySound_Implementation()
{
	if(CheckPointSound)
		UGameplayStatics::PlaySound2D(this, CheckPointSound);
}

void AISeeMeCharacter::OpenMenu()
{
	if (APlayerController* PC = GetController<APlayerController>())
	{
		if (AISMHUD* ISMHUD = Cast<AISMHUD>(PC->GetHUD()))
			ISMHUD->ToggleInGameMenu(PC);
	}
}

void AISeeMeCharacter::EnableAudio()
{
	if (AudioComponent)
	{
		AudioComponent->FadeIn(0.5f); 
	}
}

void AISeeMeCharacter::DisableAudio()
{
	if (AudioComponent)
	{
		AudioComponent->FadeOut(0.5f, 0.0f); 
	}
}
