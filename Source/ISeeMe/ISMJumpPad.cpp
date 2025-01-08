// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMJumpPad.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"

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

	ShakeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FocusTimeline"));
}

// Called when the game starts or when spawned
void AISMJumpPad::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		OnActorBeginOverlap.AddDynamic(this, &AISMJumpPad::MulticastLaunchCharacter);
	}

	if (ShakeCurve)
	{
		FOnTimelineFloat OnTimelineFloat;
		OnTimelineFloat.BindUFunction(this, TEXT("PlayShakeTimeline"));

		ShakeTimeline->AddInterpFloat(ShakeCurve, OnTimelineFloat);
		ShakeTimeline->SetLooping(false);
		ShakeTimeline->SetIgnoreTimeDilation(true);
	}
}

void AISMJumpPad::PlayShakeTimeline(float Value)
{
	FVector NextLocation = FVector(OriginalLocation.X, OriginalLocation.Y, OriginalLocation.Z + Value * 50.f);

	JumpPad->SetRelativeLocation(NextLocation);
}

void AISMJumpPad::MulticastLaunchCharacter_Implementation(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		Character->LaunchCharacter(LaunchVelocity, false, true);
	}

	OriginalLocation = JumpPad->GetRelativeLocation();

	if (ShakeTimeline)
		ShakeTimeline->PlayFromStart();
}

