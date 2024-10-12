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

	if (!bIsCrack)
	{
		RemainTime -= DeltaTime;

		if (RemainTime <= 0.f && crackStep == 3)
		{
			OnCracked();

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
				this->ResetTimer();
				}, DormantTime, false);
		}
		else if (RemainTime / CrackTime <= 0.2f && crackStep == 2)
		{
			crackStep++;
			MulticastChangeCrack(MostCrackMaterial);
			UE_LOG(LogTemp, Warning, TEXT("Most!"));
		}
		else if (RemainTime / CrackTime <= 0.5f && crackStep == 1)
		{
			crackStep++;
			MulticastChangeCrack(HalfCrackMaterial);
			UE_LOG(LogTemp, Warning, TEXT("Half!"));
		}
	}
}

void AISMCrackGround::OnStep(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority())
		return;

	UE_LOG(LogTemp, Warning, TEXT("On Step!"));

	SetActorTickEnabled(true);
	MulticastSetCracking(true);
}

void AISMCrackGround::OnCracked()
{
	MulticastCrackAwake(true);
	MulticastSpawnCrackPart();

	bIsCrack = true;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
		this->CrackDestroyTimer();
		}, CrackLifeTime, false);


	SetActorTickEnabled(true);
	MulticastAwake(false);
}

void AISMCrackGround::ResetTimer()
{
	RemainTime = CrackTime;

	SetActorTickEnabled(false);

	MulticastAwake(true);
	MulticastSetCracking(false);

	crackStep = 1;
}

void AISMCrackGround::CrackDestroyTimer()
{
	if (CrackGeometry && GroundMesh)
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
		bIsCrack = false;
	}
}

void AISMCrackGround::MulticastAwake_Implementation(bool BInAwake)
{
	if (GroundMesh)
	{
		GroundMesh->SetVisibility(BInAwake);
		GroundMesh->SetCollisionEnabled(BInAwake ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}

void AISMCrackGround::MulticastCrackAwake_Implementation(bool BInAwake)
{
	if (CrackGeometry)
	{
		CrackGeometry->SetSimulatePhysics(BInAwake);
		CrackGeometry->SetEnableGravity(BInAwake);
		CrackGeometry->SetVisibility(BInAwake);
		CrackGeometry->SetCollisionEnabled(BInAwake ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
}

void AISMCrackGround::MulticastSetCracking_Implementation(bool BInCracking)
{
	if (GroundMesh)
		GroundMesh->SetMaterial(0, BInCracking ? FirstCrackMaterial : BaseMaterial);
}

void AISMCrackGround::MulticastChangeCrack_Implementation(UMaterialInterface* ChangeMaterial)
{
	if (GroundMesh)
	{
		GroundMesh->SetMaterial(0, ChangeMaterial);
	}
}

void AISMCrackGround::MulticastSpawnCrackPart_Implementation()
{
	FVector Location = GetActorLocation() + FVector(0.0f, 0.0f, 59.0f);  

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	FRotator Rotation = FRotator::ZeroRotator;

	GetWorld()->SpawnActor<AFieldSystemActor>(CrackPartClass, Location, Rotation, SpawnParams);
}