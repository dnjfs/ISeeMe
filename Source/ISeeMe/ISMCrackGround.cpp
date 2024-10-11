// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCrackGround.h"

#include "Components/StaticMeshComponent.h"

AISMCrackGround::AISMCrackGround()
{
	PrimaryActorTick.bCanEverTick = true;

	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));
	GroundMesh->OnComponentHit.AddDynamic(this, &AISMCrackGround::OnStep);

	RootComponent = GroundMesh;

	CrackGeometry = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("CrackGeometry"));
	CrackGeometry->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	CrackGeometry->SetSimulatePhysics(false);
	CrackGeometry->SetEnableGravity(false);
	CrackGeometry->SetVisibility(false);
}

void AISMCrackGround::BeginPlay()
{
	Super::BeginPlay();

	CrackGeometry->SetWorldLocation(RootComponent->GetComponentLocation());
	CrackGeometry->SetWorldRotation(RootComponent->GetComponentRotation());

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
	else if (RemainTime / CrackTime <= 0.5f)
	{
		MulticastSetClack(RemainTime / CrackTime);
	}

}

void AISMCrackGround::OnStep(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority())
		return;

	UE_LOG(LogTemp, Warning, TEXT("On Step!"));

	SetActorTickEnabled(true);
	MulticastSetClacking(true);
}

void AISMCrackGround::OnCracked()
{
	// [TODO] 완전히 부서지는 연출
	CrackGeometry->SetVisibility(true);
	CrackGeometry->SetEnableGravity(true);
	CrackGeometry->SetSimulatePhysics(true);
	CrackGeometry->AddRadialImpulse(GroundMesh->GetComponentLocation(), 1000, 11250.0, RIF_Constant, false);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
		this->CrackTimer();
		}, CrackTime, false);


	SetActorTickEnabled(false);
	MulticastAwake(false);
}

void AISMCrackGround::ResetTimer()
{
	RemainTime = CrackTime;

	SetActorTickEnabled(false);

	MulticastAwake(true);
	MulticastSetClacking(false);
}

void AISMCrackGround::CrackTimer()
{
	if (CrackGeometry)
	{
		// 기존 컴포넌트의 Transform 저장
		FTransform OriginalTransform = CrackGeometry->GetComponentTransform();
		TObjectPtr<UGeometryCollection> SavedRestCollection = DuplicateObject<UGeometryCollection>(CrackGeometry->RestCollection, nullptr);

		// 기존 컴포넌트 파괴
		CrackGeometry->DestroyComponent();
		CrackGeometry = nullptr;

		UGeometryCollectionComponent* NewGeometryCollectionComponent = NewObject<UGeometryCollectionComponent>(this);
		CrackGeometry = NewGeometryCollectionComponent;

		// 새로운 컴포넌트 생성
		CrackGeometry->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		CrackGeometry->SetWorldTransform(OriginalTransform);

		// RestCollection 설정
		if (SavedRestCollection)
		{
			CrackGeometry->SetRestCollection(SavedRestCollection);
		}

		CrackGeometry->RegisterComponent();

		CrackGeometry->SetSimulatePhysics(false);
		CrackGeometry->SetEnableGravity(false);
		CrackGeometry->SetVisibility(false);
		GroundMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AISMCrackGround::MulticastAwake_Implementation(bool bInAwake)
{
	if (GroundMesh)
	{
		GroundMesh->SetVisibility(bInAwake);
		GroundMesh->SetCollisionEnabled(bInAwake ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}

void AISMCrackGround::MulticastSetClacking_Implementation(bool bInCracking)
{
	if (GroundMesh)
		GroundMesh->SetMaterial(0, bInCracking ? CrackingMaterial : BaseMaterial);
}

void AISMCrackGround::MulticastSetClack_Implementation(float crackState)
{
	if (GroundMesh)
	{
		if (crackState < 0.2f)
		{
			GroundMesh->SetMaterial(0, MostCrackMaterial);
		}
		else if (crackState < 0.5f)
		{
			GroundMesh->SetMaterial(0, HalfCrackMaterial);
		}
	}
}