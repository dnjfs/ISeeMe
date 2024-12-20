// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMDeadZone.h"
#include "ISMCharacterState.h"
#include "ISeeMeCharacter.h"
#include "ISMPlayerController.h"

// Sets default values
AISMDeadZone::AISMDeadZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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

void AISMDeadZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AISeeMeCharacter* Character = Cast<AISeeMeCharacter>(OtherActor))
		if (AISMPlayerController* Controller = Cast<AISMPlayerController>(Character->GetController()))
			Controller->DeadCharacter();
}
