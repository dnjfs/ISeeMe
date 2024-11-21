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
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	FocusTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FocusTimeline"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AISeeMeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// 카메라 각도 고정
	if (auto PC = GetController())
		PC->SetControlRotation(FRotator(-15, 0, 0));

	// 내 캐릭터 강조를 위해 '커스텀 뎁스 패스 렌더(bRenderCustomDepth)' 켜기
	if (GetController() && GetController()->IsLocalController())
		if (GetMesh())
			GetMesh()->SetRenderCustomDepth(true);

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
			});
	}

	if (InitVoiceChat())
	{
		LOG_SCREEN("Init voice chat success");
	}
	else
	{
		LOG_SCREEN("Init voice chat fail");
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

bool AISeeMeCharacter::InitVoiceChat()
{
	if (IOnlineSubsystem* OSS = IOnlineSubsystem::Get())
	{
		if (OnlineVoicePtr = OSS->GetVoiceInterface())
		{
			if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
			{
				if (OnlineVoicePtr->RegisterRemoteTalker(*LocalPlayer->GetPreferredUniqueNetId()))
				{
					return true;
				}
			}
		}
	}
	return false;
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

		// ToggleVoice
		EnhancedInputComponent->BindAction(ToggleVoiceAction, ETriggerEvent::Started, this, &AISeeMeCharacter::EnableVoice);
		EnhancedInputComponent->BindAction(ToggleVoiceAction, ETriggerEvent::Triggered, this, &AISeeMeCharacter::DisableVoice);
	
		EnhancedInputComponent->BindAction(SwapCameraAction, ETriggerEvent::Started, this, &AISeeMeCharacter::SwapCamera);
		EnhancedInputComponent->BindAction(SwapAspectAction, ETriggerEvent::Started, this, &AISeeMeCharacter::SwapAspect);

		EnhancedInputComponent->BindAction(OpenMenuAction, ETriggerEvent::Triggered, this, &AISeeMeCharacter::OpenMenu);

		EnhancedInputComponent->BindAction(GoCheckPointAction, ETriggerEvent::Started, this, &AISeeMeCharacter::CallGoCheckPoint);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
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
		//AddControllerPitchInput(LookAxisVector.Y); // 너무 어려움
	}
}

void AISeeMeCharacter::Focus()
{
	if (AController* MyController = GetController())
	{
		if (AISMPlayerController* ISMPlayerController = Cast<AISMPlayerController>(MyController))
		{
			if (ACharacter* OtherCharacter = ISMPlayerController->GetOtherCharacter())
			{
				FocusStartRotator = ISMPlayerController->GetControlRotation();
				FocusEndRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), OtherCharacter->GetActorLocation());
				
				FocusTimeline->PlayFromStart();
			}
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

void AISeeMeCharacter::EnableVoice()
{
	if (OnlineVoicePtr)
	{
		if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
		{
			OnlineVoicePtr->StartNetworkedVoice(LocalPlayer->GetLocalPlayerIndex());
		}
	}
}

void AISeeMeCharacter::DisableVoice()
{
	if (OnlineVoicePtr)
	{
		if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
		{
			OnlineVoicePtr->StopNetworkedVoice(LocalPlayer->GetLocalPlayerIndex());
		}
	}
}

void AISeeMeCharacter::SwapCamera()
{	
	if (HasAuthority()) // Swap Camera for Server
	{
		if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
			GM->SwapCamera();
	} 
	else if(AISMPlayerController* PC = Cast<AISMPlayerController>(GetController()))// Swap Camera for Client
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
			PC->ChangeUnHideBone(PC->OtherCharacter);
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

void AISeeMeCharacter::GoCheckPoint()
{
	TArray<AISeeMeCharacter*> Characters;
	TArray<AISMCheckPoint*> CheckPoints;
	TArray<AISMCharacterState*> States;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		if(ACharacter* BaseCharacter = PC->GetCharacter())
			if (AISeeMeCharacter* Character = Cast<AISeeMeCharacter>(BaseCharacter))
			{
				Characters.Add(Character);
				if (AISMCharacterState* State = Cast<AISMCharacterState>(Character->GetPlayerState()))
				{
					States.Add(State);
					if (AISMCheckPoint* CheckPoint = State->CurCheckPoint)
						CheckPoints.Add(CheckPoint);
				}
			}
	}

	// Go back Check Point
	if(Characters.Num() == 2)
		if (CheckPoints.Num() == 2)
		{
			Characters[0]->SetActorLocation(CheckPoints[0]->Spawn1PPlayer->GetComponentLocation());
			Characters[1]->SetActorLocation(CheckPoints[1]->Spawn2PPlayer->GetComponentLocation());
		}
}

void AISeeMeCharacter::ServerCallGoCheckPoint_Implementation()
{
	GoCheckPoint(); //  Call Go Check Point Function from client to the server
}

void AISeeMeCharacter::OpenMenu()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AHUD* HUD = PC->GetHUD())
		{
			if (AISMHUD* ISMHUD = Cast<AISMHUD>(HUD))
			{
				ISMHUD->ToggleInGameMenu(PC);
			}
		}
	}
}

