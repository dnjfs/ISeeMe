// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMDeadZone.h"
#include "ISMCharacterState.h"
#include "ISeeMeCharacter.h"

// Sets default values
AISMDeadZone::AISMDeadZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	if (TriggerVolume == nullptr)
		return;

	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TriggerVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, HasAuthority() ? ECR_Overlap : ECR_Ignore);
	TriggerVolume->SetGenerateOverlapEvents(true);

	RootComponent = TriggerVolume;
}

// Called when the game starts or when spawned
void AISMDeadZone::BeginPlay()
{
	Super::BeginPlay();
	
	if (TriggerVolume)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AISMDeadZone::OnOverlapBegin);
	}
}

// Called every frame
void AISMDeadZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AISMDeadZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AISeeMeCharacter* Character = Cast<AISeeMeCharacter>(OtherActor))
		if (AISMCharacterState* State = Cast<AISMCharacterState>(Character->GetPlayerState()))
		{
			if (!HasAuthority())
				return;

			if (State->CurCheckPoint)
			{
				if (State->CustomPlayerID == 1)
				{
					Character->SetActorLocation(State->CurCheckPoint->Spawn1PPlayer->GetComponentLocation());
					Character->SetActorRotation(State->CurCheckPoint->Spawn1PPlayer->GetComponentRotation());
				}
				else if (State->CustomPlayerID == 2)
				{
					Character->SetActorLocation(State->CurCheckPoint->Spawn2PPlayer->GetComponentLocation());
					Character->SetActorRotation(State->CurCheckPoint->Spawn2PPlayer->GetComponentRotation());
				}
			} // When get check point
			else
			{
				Character->SetActorLocation(State->InitSpawnPointLocation);
				Character->SetActorRotation(State->InitialSpawnPointRotator);
			} // When don't get check point
		}
}
