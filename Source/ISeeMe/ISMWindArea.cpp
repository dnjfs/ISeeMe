// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMWindArea.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include <Kismet/GameplayStatics.h>
#include "ISeeMeCharacter.h"

// Sets default values
AISMWindArea::AISMWindArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Volume = CreateDefaultSubobject<UBoxComponent>(FName("Volume"));

	WindVector = CreateDefaultSubobject<UArrowComponent>(FName("WindVector"));
	WindVector->SetupAttachment(Volume);
	WindVector->ArrowLength = 100.f;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(Volume);
	AudioComponent->bAutoActivate = false;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AISMWindArea::BeginPlay()
{
	Super::BeginPlay();

	AudioComponent->Stop();

	Volume->OnComponentBeginOverlap.AddDynamic(this, &AISMWindArea::OnEnter);
	Volume->OnComponentEndOverlap.AddDynamic(this, &AISMWindArea::OnExit);

	if (WindEffect)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(WindEffect, Volume, TEXT("Effect"),
			WindVector->GetComponentLocation(), WindVector->GetComponentRotation(), EAttachLocation::KeepWorldPosition, true);

		NiagaraComponent->SetNiagaraVariableVec3(TEXT("Box Size"), Volume->GetScaledBoxExtent());
	}
}

// Called every frame
void AISMWindArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
		return;

	MulticastApplyWindForce(DeltaTime);
}

void AISMWindArea::OnEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor))
	{
		if (APlayerController* PC = Cast<APlayerController>(OverlappingCharacter->GetController()))
		{
			if (PC->IsLocalController())
			{
				if (AudioComponent)
				{
					AudioComponent->SetSound(WindSound);
					AudioComponent->FadeIn(1.f);
				}
			}
		}
	}

	if (!HasAuthority())
		return;

	MulticastAddTarget(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AISMWindArea::OnExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor))
	{
		if (APlayerController* PC = Cast<APlayerController>(OverlappingCharacter->GetController()))
		{
			if (PC->IsLocalController())
			{
				if (AudioComponent)
				{
					AudioComponent->FadeOut(1.f, 0.f);
				}
			}
		}
	}

	if (!HasAuthority())
		return;

	MulticastRemoveTarget(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AISMWindArea::MulticastApplyWindForce_Implementation(float DeltaTime)
{
	for (int32 i = 0; i < Targets.Num(); i++)
	{
		if (i >= Targets.Num()) return; // array changed during iteration 예외 방지

		ACharacter* Target = Targets[i];

		FVector CurrLocation = Target->GetActorLocation();
		FVector NextLocation = CurrLocation + (WindVector->GetForwardVector() * WindVector->ArrowLength * DeltaTime);
		Target->SetActorLocation(NextLocation);
	}
}

void AISMWindArea::MulticastAddTarget_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		Targets.Add(Character);
	}
}

void AISMWindArea::MulticastRemoveTarget_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		Targets.Remove(Character);
	}
}
