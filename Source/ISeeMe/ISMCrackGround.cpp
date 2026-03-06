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
		if (CrackState < ECrackState::Cracked)
		{
			CrackState = ECrackState::Cracked;
			OnCracked();
		}
	}
	else if (RemainTime / CrackTime <= 0.2f)
	{
		if (CrackState < ECrackState::Most)
		{
			CrackState = ECrackState::Most;
			MulticastChangeCrack(MostCrack);
		}
	}
	else if (RemainTime / CrackTime <= 0.5f)
	{
		if (CrackState < ECrackState::Half)
		{
			CrackState = ECrackState::Half;
			MulticastChangeCrack(HalfCrack);
		}
	}
}

void AISMCrackGround::OnReset()
{
	ResetTimer();
}

void AISMCrackGround::OnStep(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	if (!OtherActor->IsA<AISeeMeCharacter>())
		return;

	if (IsActorTickEnabled())
	{
		// 한 번 밟히고 나면 이미 틱이 활성화 된 상태임
		return;
	}

	CrackState = ECrackState::Start;

	MulticastSetCracking(CrackState);
	SetActorTickEnabled(true);
}

void AISMCrackGround::OnCracked()
{
	MulticastSpawnCrackPart();

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindWeakLambda(this, [this]() {
		if (IsValid(this))
			this->ResetTimer();
	});

	GetWorld()->GetTimerManager().SetTimer(CrackTimerHandle, TimerDelegate, DormantTime, false);

	MulticastAwake(false);
}

void AISMCrackGround::ResetTimer()
{
	RemainTime = CrackTime;
	CrackState = ECrackState::Idle;

	MulticastAwake(true);
	MulticastSetCracking(CrackState);
	SetActorTickEnabled(false);

	GetWorld()->GetTimerManager().ClearTimer(CrackTimerHandle);
}

void AISMCrackGround::MulticastAwake_Implementation(bool bInAwake)
{
	if (GroundMesh)
		GroundMesh->SetCollisionEnabled(bInAwake ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

	if (BoxCollision)
		BoxCollision->SetCollisionEnabled(bInAwake ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

	if (bInAwake)
	{
		if (AudioComponent)
			AudioComponent->Stop();
	}
}

void AISMCrackGround::MulticastSetCracking_Implementation(ECrackState InCrackState)
{
	bool bIsCracking = InCrackState > ECrackState::Idle;
	if (GroundMesh)
		GroundMesh->SetVisibility(!bIsCracking);

	if (GeometryCacheComp)
	{
		GeometryCacheComp->SetVisibility(bIsCracking);
		GeometryCacheComp->Stop();
		GeometryCacheComp->SetLooping(true);
		GeometryCacheComp->SetGeometryCache(bIsCracking ? FirstCrack : nullptr);
		GeometryCacheComp->PlayFromStart();
	}

	if (bIsCracking && AudioComponent)
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

		// 애니메이션 길이보다 대기시간이 더 길면 애니메이션 종료 시 제거
		float AnimDuration = FallingGeometry->CalculateDuration();
		if (AnimDuration < DormantTime)
		{
			FTimerHandle TimerHandle;
			TWeakObjectPtr<UGeometryCacheComponent> WeakGeometryCacheComp(GeometryCacheComp);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [WeakGeometryCacheComp]() {
				if (TStrongObjectPtr<UGeometryCacheComponent> StrongGeometryCacheComp = WeakGeometryCacheComp.Pin(false))
				{
					StrongGeometryCacheComp->SetGeometryCache(nullptr);
				}
			}, AnimDuration, false);
		}
	}

	if (AudioComponent)
	{
		AudioComponent->SetSound(BrokenSound);
		AudioComponent->Play();
	}
}