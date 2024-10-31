// Fill out your copyright notice in the Description page of Project Settings.


#include "PracticeTrigger.h"
#include "CinematicUI.h"
#include "ISeeMeCharacter.h"
#include "Blueprint/UserWidget.h"
#include <Runtime/LevelSequence/Public/LevelSequencePlayer.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputDeveloperSettings.h>
#include <EnhancedInputComponent.h>

// Sets default values
APracticeTrigger::APracticeTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolume"));
	if (TriggerVolume == nullptr)
		return;

	RootComponent = TriggerVolume;
}

// Called when the game starts or when spawned
void APracticeTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APracticeTrigger::OnOverlapBegin);

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		EnableInput(PlayerController);

		// 입력 구성
		if (InputComponent)
		{
			InputComponent->BindAction("ActivateK", IE_Pressed, this, &APracticeTrigger::ChargeSkip);
		}
	}
}

// Called every frame
void APracticeTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LevelSequencePlayer && LevelSequencePlayer->IsPlaying() && UIWidgetInstance->curProgress == 10)
	{
		LevelSequencePlayer->Stop();
		UE_LOG(LogTemp, Warning, TEXT("Level sequence stopped."));
		ActivateInput();
	}
}

void APracticeTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	UIWidgetInstance = CreateWidget<UCinematicUI>(GetWorld(), UIWidgetClass);
	if (UIWidgetInstance != nullptr)
	{
		UIWidgetInstance->AddToViewport();
	}

	ALevelSequenceActor* OutLevelSequenceActor = nullptr;  // LevelSequenceActor를 받을 변수 선언
	LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), LevelSequence, FMovieSceneSequencePlaybackSettings(), OutLevelSequenceActor);

	if (LevelSequencePlayer)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				APlayerController* PlayerController = Iterator->Get();

				if (PlayerController)
				{
					APawn* ControlledPawn = PlayerController->GetPawn();
					AISeeMeCharacter* Character = Cast<AISeeMeCharacter>(OtherActor);
					if (ControlledPawn)
					{
						ControlledPawn->DisableInput(PlayerController);
					}
				}
			}
		}
		LevelSequencePlayer->OnFinished.AddDynamic(this, &APracticeTrigger::ActivateInput);
		LevelSequencePlayer->Play();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to create level sequence player"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Activated"));
}

void APracticeTrigger::ActivateInput()
{
	UIWidgetInstance->RemoveFromParent();

	UWorld* World = GetWorld();
	if (World)
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (PlayerController)
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayerController found"));

				APawn* ControlledPawn = PlayerController->GetPawn();
				if (ControlledPawn)
				{
					ControlledPawn->EnableInput(PlayerController);
					UE_LOG(LogTemp, Warning, TEXT("Enabled input for ControlledPawn"));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("World is null"));
	}
}

void APracticeTrigger::ChargeSkip()
{
	UE_LOG(LogTemp, Warning, TEXT("Charge Skip"));
	if (UIWidgetInstance)
	{
		UIWidgetInstance->curProgress += 1;
		UIWidgetInstance->SetSkip(UIWidgetInstance->curProgress/10.0, 100.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Widget"));
	}
}

void APracticeTrigger::Skip()
{
	if (LevelSequencePlayer && LevelSequencePlayer->IsPlaying() && UIWidgetInstance->curProgress==10)
	{
		LevelSequencePlayer->Stop();
		UE_LOG(LogTemp, Warning, TEXT("Level sequence stopped."));
		ActivateInput();
	}
	ActivateInput();
}

