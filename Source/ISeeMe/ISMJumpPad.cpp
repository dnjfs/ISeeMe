// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMJumpPad.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"

// Sets default values
AISMJumpPad::AISMJumpPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	JumpPad = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JumpPad"));
	JumpPad->SetCollisionProfileName(FName("OverlapAllDynamic"));

	LaunchVelocity = FVector(0.f, 0.f, 1000.f);

	RootComponent = JumpPad;
}

// Called when the game starts or when spawned
void AISMJumpPad::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		OnActorBeginOverlap.AddDynamic(this, &AISMJumpPad::LaunchCharacter);
	}
}

void AISMJumpPad::LaunchCharacter_Implementation(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		Character->LaunchCharacter(LaunchVelocity, false, true);
	}
}

