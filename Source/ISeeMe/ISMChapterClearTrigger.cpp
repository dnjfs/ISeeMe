// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMChapterClearTrigger.h"
#include "Components/BoxComponent.h"
#include "ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "ISMGameInstance.h"
#include "GameFramework/Character.h"
#include "ISMPlayerController.h"
#include "ISeeMe/UI/ISMOverlay.h"
#include "Components/WidgetSwitcher.h" 
#include <GameFramework/HUD.h>
#include "ISeeMe/UI/ISMHUD.h"
#include "ISMLobbyController.h"
#include <Net/UnrealNetwork.h>


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
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AISMChapterClearTrigger::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AISMChapterClearTrigger::OnOverlapEnd);
}

// Called every frame
void AISMChapterClearTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AISMChapterClearTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	MulticastDetectPlayer(true, OtherActor);
	// When all detect
	if (DetectedPlayerCount == 2)
	{
		CompleteChapter();
	}
}

void AISMChapterClearTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	MulticastDetectPlayer(false,OtherActor);
}

void AISMChapterClearTrigger::MulticastDetectPlayer_Implementation(bool bAdd, AActor* OtherActor)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (bAdd)
			DetectedPlayerCount++;
		else
			DetectedPlayerCount--;
	}
}

void AISMChapterClearTrigger::CompleteChapter()
{
	if (OnClearUpdated.IsBound())
	{
		OnClearUpdated.Execute(true);
	} // Delegate Execute (Loading UI)

	MulticastPlaySound();
	MoveToNextChapter();
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Chapter Clear"));
	MulticastSaveChapterNo();
}

void AISMChapterClearTrigger::MulticastPlaySound_Implementation()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		GI->SoundPlay(ClearSound);
	}
}

void AISMChapterClearTrigger::MoveToNextChapter()
{
	UWorld* World = GetWorld(); 

	if (HasAuthority())
	{
		World->ServerTravel("/Game/ISeeMe/Maps/" + NextChapter + "?listen", true);
	}
}

void AISMChapterClearTrigger::MulticastSaveChapterNo_Implementation()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		GI->MaxChapterNo = FMath::Max(GI->MaxChapterNo, NextChapterNo);
		GI->CurrChapterNo = NextChapterNo;
		GI->SavedCheckPointID = FName("None");

		GI->SaveGame();
	}
}


