// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMRecollection.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ISMGameInstance.h"
#include "ISeeMeCharacter.h"

// Sets default values
AISMRecollection::AISMRecollection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Volume"));
	TriggerVolume->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AISMRecollection::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
		{
			if (GI->AcquiredRecollectionIDs.Contains(this->GetFName()))
			{
				MulticastDestroyRecollection();
			}
		}
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AISMRecollection::OnOverlap);
	}
}

void AISMRecollection::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AISeeMeCharacter>(OtherActor))
	{
		MulticastAcquireRecollection();
		MulticastDestroyRecollection();
	}
}

// Called every frame
void AISMRecollection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AISMRecollection::MulticastAcquireRecollection_Implementation()
{
	// save recollection info
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		GI->AcquiredRecollectionIDs.AddUnique(this->GetFName());
		GI->SaveGame();
	}
}

void AISMRecollection::MulticastDestroyRecollection_Implementation()
{
	this->Destroy();
}