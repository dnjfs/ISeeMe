// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMChapterClearTrigger.h"
#include "Components/BoxComponent.h"
#include "ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "ISMGameInstance.h"
#include "GameFramework/Character.h"

// Sets default values
AISMChapterClearTrigger::AISMChapterClearTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
}

// Called when the game starts or when spawned
void AISMChapterClearTrigger::BeginPlay()
{
	Super::BeginPlay();

	DetectedPlayerCount = 0;
	
	if (HasAuthority())
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AISMChapterClearTrigger::OnOverlapBegin);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AISMChapterClearTrigger::OnOverlapEnd);
	}
}

// Called every frame
void AISMChapterClearTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AISMChapterClearTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		DetectedPlayerCount++;
	}

	// When all detect
	if (DetectedPlayerCount == 2)
	{
		CompleteChapter();
	}
}

void AISMChapterClearTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		DetectedPlayerCount--;
	}
}

void AISMChapterClearTrigger::CompleteChapter()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Chapter Clear"));
	MulticastSaveChapterNo();
}

void AISMChapterClearTrigger::MulticastSaveChapterNo_Implementation()
{
	if (UISMSaveGame* SaveGameInstance = Cast<UISMSaveGame>(UGameplayStatics::CreateSaveGameObject(UISMSaveGame::StaticClass())))
	{
		if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
		{
			GI->CurrChapterNo++;
			SaveGameInstance->CurrChapterNo = GI->CurrChapterNo;
			SaveGameInstance->CheckPointID = FName("None");

			UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, TEXT("SaveSlot"), 0);
		}
	}
}

void AISMChapterClearTrigger::MoveToNextChapter()
{
	//TODO
}

