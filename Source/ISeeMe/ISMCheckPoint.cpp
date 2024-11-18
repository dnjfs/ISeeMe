// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCheckPoint.h"
#include <Components/BoxComponent.h>
#include "GameFramework/Character.h"
#include "ISMCharacterState.h"
#include "ISMPlayerController.h"

// Sets default values
AISMCheckPoint::AISMCheckPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CheckPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckPointMesh"));
	if (CheckPointMesh == nullptr)
		return;

	RootComponent = CheckPointMesh;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	if (TriggerVolume == nullptr)
		return;

	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TriggerVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, HasAuthority() ? ECR_Overlap : ECR_Ignore);
	TriggerVolume->SetGenerateOverlapEvents(true);

	TriggerVolume->SetupAttachment(CheckPointMesh);

	Spawn1PPlayer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawn 1P Player"));
	if (Spawn1PPlayer == nullptr)
		return;
	Spawn1PPlayer->SetupAttachment(CheckPointMesh);

	Spawn2PPlayer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawn 2P Player"));
	if (Spawn2PPlayer == nullptr)
		return;
	Spawn2PPlayer->SetupAttachment(CheckPointMesh);
}

// Called when the game starts or when spawned
void AISMCheckPoint::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerVolume)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AISMCheckPoint::OnOverlapBegin);
		TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AISMCheckPoint::OnOverlapEnd);
	}

	MulticastChangeMaterial(DetectPlayer);

	DetectPlayer = 0;
}

// Called every frame
void AISMCheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AISMCheckPoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
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
				State->bCheckPoint = true;
				DetectPlayer++;
				MulticastChangeMaterial(DetectPlayer);
			}

			// When all detect
			if (DetectPlayer == 2)
			{
				MulticastChangeMaterial(DetectPlayer);
				DetectPlayer++;
				
				if (HasAuthority())
					InitCheckPoint();
				else
					ServerInitCheckPoint();
			}
		}
	}
}

void AISMCheckPoint::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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
			if (DetectPlayer != 2 && State->bCheckPoint)
			{
				State->bCheckPoint = false;
				DetectPlayer--;
				MulticastChangeMaterial(DetectPlayer);
			}
		}
	}
}

void AISMCheckPoint::MulticastChangeMaterial_Implementation(int CurDetect)
{
	// According to the perception of a person
	if (CheckPointMesh)
	{
		if (CurDetect == 0 && BaseMaterial)
		{
			CheckPointMesh->SetMaterial(0, BaseMaterial);
		}
		else if(CurDetect == 1 && CheckMaterial)
		{
			CheckPointMesh->SetMaterial(0, CheckMaterial);
		}
		else if (CurDetect == 2 && AllCheckMaterial)
		{
			CheckPointMesh->SetMaterial(0, AllCheckMaterial);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Mesh"));
	}
}

void AISMCheckPoint::InitCheckPoint()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get()))
		{
			if (ACharacter* Character = Cast<ACharacter>(PC->GetCharacter()))
			{
				if (AISMCharacterState* State = Cast<AISMCharacterState>(Character->GetPlayerState()))
				{
					State->bCheckPoint = false;
					State->CurCheckPoint = this;
				}
			}
		}
	}
}

void AISMCheckPoint::ServerInitCheckPoint_Implementation()
{
	InitCheckPoint();
}
