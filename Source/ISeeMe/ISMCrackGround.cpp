// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCrackGround.h"

#include "Components/StaticMeshComponent.h"

AISMCrackGround::AISMCrackGround()
{
	PrimaryActorTick.bCanEverTick = true;

	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));
	GroundMesh->OnComponentHit.AddDynamic(this, &AISMCrackGround::OnStep);

	RootComponent = GroundMesh;
}

void AISMCrackGround::BeginPlay()
{
	Super::BeginPlay();

	ResetTimer();
}

void AISMCrackGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RemainTime -= DeltaTime;

	// [TODO] 점점 부서지는 연출
	if (RemainTime <= 0.f)
	{
		OnCracked();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
			this->ResetTimer();
		}, DormantTime, false);
	}
}

void AISMCrackGround::OnStep(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority())
		return;

	UE_LOG(LogTemp, Warning, TEXT("On Step!"));

	SetActorTickEnabled(true);
}

void AISMCrackGround::OnCracked()
{
	// [TODO] 완전히 부서지는 연출

	SetActorTickEnabled(false);
	MulticastAwake(false);
}

void AISMCrackGround::ResetTimer()
{
	RemainTime = CrackTime;

	SetActorTickEnabled(false);

	MulticastAwake(true);
}

void AISMCrackGround::MulticastAwake_Implementation(bool bInAwake)
{
	if (GroundMesh)
	{
		GroundMesh->SetVisibility(bInAwake);
		GroundMesh->SetCollisionEnabled(bInAwake ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}