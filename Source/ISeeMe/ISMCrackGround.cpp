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
	CrackGeometry->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MulticastCrackAwake(false);
}

void AISMCrackGround::BeginPlay()
{
	Super::BeginPlay();

	CrackGeometry->SetRelativeScale3D(FVector(1.0 / GroundMesh->GetComponentScale().X, 1.0 / GroundMesh->GetComponentScale().Y,
		1.0 / GroundMesh->GetComponentScale().Z));

	ResetTimer();
}

void AISMCrackGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isCrack)
	{
		RemainTime -= DeltaTime;

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
	MulticastCrackAwake(true);
	CrackGeometry->AddRadialImpulse(GetActorLocation(), 1000, 25000.0, RIF_Constant, false);

	/*FVector Location = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 10);
	FRotator Rotation = FRotator::ZeroRotator;  // 원하는 회전값 설정 (기본값으로 0도)

	// BP_CrackPart 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	ACrackPart* SpawnedCrackPart = GetWorld()->SpawnActor<ACrackPart>(CrackPartClass, Location, Rotation, SpawnParams);
	*/
	isCrack = true;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
		this->CrackTimer();
		}, CrackTime, false);


	SetActorTickEnabled(true);
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
		TObjectPtr<UGeometryCollection> SavedRestCollection = DuplicateObject<UGeometryCollection>(CrackGeometry->RestCollection, nullptr);

		CrackGeometry->DestroyComponent();
		CrackGeometry = nullptr;

		UGeometryCollectionComponent* NewGeometryCollectionComponent = NewObject<UGeometryCollectionComponent>(this);
		CrackGeometry = NewGeometryCollectionComponent;

		CrackGeometry->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		CrackGeometry->SetRelativeScale3D(FVector(1.0 / GroundMesh->GetComponentScale().X, 1.0 / GroundMesh->GetComponentScale().Y,
			1.0 / GroundMesh->GetComponentScale().Z));

		if (SavedRestCollection)
		{
			CrackGeometry->SetRestCollection(SavedRestCollection);
		}
		CrackGeometry->RegisterComponent();

		MulticastCrackAwake(false);
		SetActorTickEnabled(false);
		isCrack = false;
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

void AISMCrackGround::MulticastCrackAwake_Implementation(bool bInAwake)
{
	if (CrackGeometry)
	{
		CrackGeometry->SetSimulatePhysics(bInAwake);
		CrackGeometry->SetEnableGravity(bInAwake);
		CrackGeometry->SetVisibility(bInAwake);
		CrackGeometry->SetCollisionEnabled(bInAwake ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
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