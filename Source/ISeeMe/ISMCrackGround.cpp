// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCrackGround.h"

#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "GeometryCache.h"
#include "GeometryCacheComponent.h"

#include "ISeeMeCharacter.h"

AISMCrackGround::AISMCrackGround()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));
	RootComponent = GroundMesh;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AISMCrackGround::OnStep);
	BoxCollision->AttachToComponent(GroundMesh, FAttachmentTransformRules::KeepRelativeTransform);

	GeometryCacheComp = CreateDefaultSubobject<UGeometryCacheComponent>(TEXT("GeometryCacheComponent"));
	GeometryCacheComp->SetupAttachment(GroundMesh);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(GroundMesh);
	AudioComponent->bAutoActivate = false;
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

	if (RemainTime <= 0.f)
	{
		OnCracked();
	}
	else if (RemainTime / CrackTime <= 0.2f)
	{
		if (GeometryCacheComp && GeometryCacheComp->GetGeometryCache() == HalfCrack)
			MulticastChangeCrack(MostCrack);
	}
	else if (RemainTime / CrackTime <= 0.5f)
	{
		if (GeometryCacheComp && GeometryCacheComp->GetGeometryCache() == FirstCrack)
			MulticastChangeCrack(HalfCrack);
	}
}

void AISMCrackGround::OnStep(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	if (!OtherActor->IsA<AISeeMeCharacter>())
		return;

	SetActorTickEnabled(true);
	MulticastSetCracking(true);
}

void AISMCrackGround::OnCracked()
{
	MulticastSpawnCrackPart();

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindWeakLambda(this, [this]() {
		if (IsValid(this))
			this->ResetTimer();
	});

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DormantTime, false);

	MulticastAwake(false);
}

void AISMCrackGround::ResetTimer()
{
	RemainTime = CrackTime;

	MulticastAwake(true);
	MulticastSetCracking(false);
	
	if (AudioComponent)
	{
		AudioComponent->Stop();
		AudioComponent->SetSound(CrackingSound);
	}
}

void AISMCrackGround::MulticastAwake_Implementation(bool bInAwake)
{
	SetActorTickEnabled(false);

	if (GroundMesh)
		GroundMesh->SetCollisionEnabled(bInAwake ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

	if (BoxCollision)
		BoxCollision->SetCollisionEnabled(bInAwake ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AISMCrackGround::MulticastSetCracking_Implementation(bool bInCracking)
{
	if (GroundMesh)
		GroundMesh->SetVisibility(!bInCracking);

	if (GeometryCacheComp)
	{
		GeometryCacheComp->SetVisibility(bInCracking);
		GeometryCacheComp->Stop();
		GeometryCacheComp->SetLooping(true);
		GeometryCacheComp->SetGeometryCache(bInCracking ? FirstCrack : nullptr);
		GeometryCacheComp->PlayFromStart();
	}

	if (bInCracking && AudioComponent)
	{
		AudioComponent->SetSound(CrackingSound);
		AudioComponent->Play();
	}
}

void AISMCrackGround::MulticastChangeCrack_Implementation(UGeometryCache* ChangeGeometry)
{
	if (GeometryCacheComp)
	{
		GeometryCacheComp->Stop();
		GeometryCacheComp->SetLooping(true);
		GeometryCacheComp->SetGeometryCache(ChangeGeometry);
		GeometryCacheComp->PlayFromStart();
	}
}

void AISMCrackGround::MulticastSpawnCrackPart_Implementation()
{
	if (GeometryCacheComp)
	{
		GeometryCacheComp->Stop();
		GeometryCacheComp->SetLooping(false);
		GeometryCacheComp->SetGeometryCache(FallingGeometry);
		GeometryCacheComp->PlayFromStart();
	}

	if (AudioComponent)
	{
		AudioComponent->SetSound(BrokenSound);
		AudioComponent->Play();
	}
}