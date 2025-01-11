// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMSwitch.h"
#include "GameFramework/Character.h"

// Sets default values
AISMSwitch::AISMSwitch()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
	if (SwitchMesh == nullptr)
		return;
	SwitchMesh->SetupAttachment(RootComponent);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	if (TriggerVolume == nullptr)
		return;
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TriggerVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, HasAuthority() ? ECR_Overlap : ECR_Ignore);
	TriggerVolume->SetGenerateOverlapEvents(true);
	TriggerVolume->SetupAttachment(SwitchMesh);

	MoveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoveMesh"));
	if (MoveMesh == nullptr)
		return;
	MoveMesh->SetupAttachment(RootComponent);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AISMSwitch::BeginPlay()
{
	Super::BeginPlay();

	// Init Location and Speed
	StartLocation = MoveMesh->GetComponentLocation();
	EndLocation = StartLocation + DirectionVector;
	TargetLocation = EndLocation;
	MoveSpeed = FVector::Dist(StartLocation, EndLocation) / MoveTime;

	MulticastChangeMaterial(false);
	if (TriggerVolume)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AISMSwitch::OnOverlapBegin);
		TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AISMSwitch::OnOverlapEnd);
	}
	SetActorTickEnabled(false);
}

// Called every frame
void AISMSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
		return;

	MulticastMove(DeltaTime);
}

void AISMSwitch::MulticastMove_Implementation(float DeltaTime)
{
	FVector CurLocation = MoveMesh->GetComponentLocation();
	float DistanceToTarget = FVector::Dist(CurLocation, TargetLocation);
	FVector WorldNextLocation = CurLocation + ((TargetLocation - CurLocation).GetSafeNormal()) * DeltaTime * MoveSpeed;
	MoveMesh->SetWorldLocation(WorldNextLocation);

	if (DistanceToTarget <= 1)
	{
		SetActorTickEnabled(false);
		return;
	}
}

void AISMSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (!HasAuthority())
		{
			return;
		}

		CurDetectPlayer++;
		if (CurDetectPlayer == 1)
		{
			TargetLocation = EndLocation;
			MulticastChangeMaterial(true);
		}
	}
}

void AISMSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (!HasAuthority())
		{
			return;
		}

		CurDetectPlayer--;
		if (CurDetectPlayer < 1)
		{
			TargetLocation = StartLocation;
			MulticastChangeMaterial(false);
		}
	}
}

void AISMSwitch::MulticastChangeMaterial_Implementation(bool bCheck)
{
	SetActorTickEnabled(true);
	if (bCheck)
	{
		SwitchMesh->SetMaterial(0, CheckMaterial);
	}
	else
	{
		SwitchMesh->SetMaterial(0, BaseMaterial);
	}
}


