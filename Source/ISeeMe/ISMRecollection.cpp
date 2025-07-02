// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMRecollection.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ISMGameInstance.h"
#include "ISeeMeCharacter.h"
#include "Manager/AchievementManager.h"

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
		// 이미 획득한 추억이라도 레벨에서 삭제하지 않도록 주석처리
		//if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
		//{
		//	if (GI->AcquiredRecollectionIDs.Contains(this->GetFName()))
		//	{
		//		MulticastDestroyRecollection();
		//	}
		//}
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

		// 도전과제 달성
		if (UAchievementManager* AM = GI->GetSubsystem<UAchievementManager>())
		{
			AM->UpdateAchievementProgress(FString::Printf(TEXT("Recollection_%d"), RecollectionID), 1.f);
		}
	}
}

void AISMRecollection::MulticastDestroyRecollection_Implementation()
{
	this->Destroy();
}