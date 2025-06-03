// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMTutorialStepDoneActor.h"
#include <Kismet/GameplayStatics.h>
#include "ISMTutorialGameState.h"
#include "ISMCharacterState.h"
#include "GameFramework/Character.h"
#include "ISMPlayerController.h"

// Sets default values
AISMTutorialStepDoneActor::AISMTutorialStepDoneActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	TriggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerVolume"));
	if (TriggerVolume == nullptr)
		return;

	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TriggerVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, HasAuthority() ? ECR_Overlap : ECR_Ignore);
	TriggerVolume->SetGenerateOverlapEvents(true);

	TriggerVolume->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AISMTutorialStepDoneActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (TriggerVolume)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AISMTutorialStepDoneActor::OnOverlapBegin);
		TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AISMTutorialStepDoneActor::OnOverlapEnd);
	}

	DetectPlayer = 0;
	InitStepDone();
}

void AISMTutorialStepDoneActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (AISMCharacterState* State = Cast<AISMCharacterState>(Character->GetPlayerState()))
		{
			if (!HasAuthority())
			{
				return;
			}

			// When one detect
			if (!State->bCheckPoint && DetectPlayer <= 1)
			{
				LOG_SCREEN("One Detect");
				State->bCheckPoint = true;
				DetectPlayer++;
			}

			// When all detect
			if (DetectPlayer == MaxDetectPlayer)
			{
				LOG_SCREEN("All Detect");
				if (AISMTutorialGameState* GS = Cast<AISMTutorialGameState>(UGameplayStatics::GetGameState(this)))
				{
					GS->MulticastInformation();
				}

				DetectPlayer++;
				InitStepDone();
			}
		}
	}
}

void AISMTutorialStepDoneActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (AISMCharacterState* State = Cast<AISMCharacterState>(Character->GetPlayerState()))
		{
			if (!HasAuthority())
			{
				return;
			}

			// When one out
			if (DetectPlayer != MaxDetectPlayer && State->bCheckPoint)
			{
				State->bCheckPoint = false;
				DetectPlayer--;
			}
		}
	}
}

void AISMTutorialStepDoneActor::InitStepDone()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get()))
		{
			if (ACharacter* Character = PC->GetCharacter())
			{
				if (AISMCharacterState* State = Cast<AISMCharacterState>(Character->GetPlayerState()))
				{
					State->bCheckPoint = false;
				}
			}
		}
	}
}

