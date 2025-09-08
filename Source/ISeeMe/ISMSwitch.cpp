// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMSwitch.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "ISeeMeCharacter.h"
#include "ISMPlayerController.h"

// Sets default values
AISMSwitch::AISMSwitch()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
	SwitchMesh->SetupAttachment(RootComponent);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TriggerVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, HasAuthority() ? ECR_Overlap : ECR_Ignore);
	TriggerVolume->SetGenerateOverlapEvents(true);
	TriggerVolume->SetupAttachment(SwitchMesh);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AISMSwitch::BeginPlay()
{
	Super::BeginPlay();

	// Init Location and Speed
	if (TargetActor)
	{
		MulticastChangeMaterial(false);
		if (TriggerVolume)
		{
			TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AISMSwitch::OnOverlapBegin);
			TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AISMSwitch::OnOverlapEnd);
		}
		SetActorTickEnabled(false);
	}
	else // TargetActor가 지정되어있지 않을 경우 이 액터 제거
	{
		Destroy();
	}
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
	FVector CurLocation = TargetActor->GetActorLocation();
	FVector WorldNextLocation = CurLocation + CurrDirection.GetSafeNormal() * DeltaTime * MoveSpeed;

	TargetActor->SetActorLocation(WorldNextLocation);

	if (bIsForward) // 정방향 이동
	{
		CurrMovedTime += DeltaTime;

		if (TotalMoveTime - CurrMovedTime <= 0.f)
		{
			SetActorTickEnabled(false);
			return;
		}
	}
	else // 역방향 이동
	{
		CurrMovedTime -= DeltaTime;

		if (CurrMovedTime <= 0.f)
		{
			SetActorTickEnabled(false);
			return;
		}
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

		if (AISeeMeCharacter* OverlappingCharacter = Cast<AISeeMeCharacter>(Character))
		{
			if (AISMPlayerController* PC = Cast<AISMPlayerController>(OverlappingCharacter->GetController()))
			{
				PC->ClientPlayLocalSound(SwitchOnSound, true);
			}
		}

		CurDetectPlayer++;
		if (CurDetectPlayer == 1)
		{
			SetActorTickEnabled(true);
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

		if (AISeeMeCharacter* OverlappingCharacter = Cast<AISeeMeCharacter>(Character))
		{
			if (AISMPlayerController* PC = Cast<AISMPlayerController>(OverlappingCharacter->GetController()))
			{
				PC->ClientPlayLocalSound(SwitchOffSound, true);
			}
		}

		CurDetectPlayer--;
		if (CurDetectPlayer < 1)
		{
			MulticastChangeMaterial(false);
		}
	}
}

void AISMSwitch::MulticastChangeMaterial_Implementation(bool bCheck)
{
	SetActorTickEnabled(true);
	if (bCheck)
	{
		CurrDirection = DirectionVector;
		bIsForward = true;
		SwitchMesh->SetMaterial(0, CheckMaterial);
	}
	else
	{
		CurrDirection = -DirectionVector;
		bIsForward = false;
		SwitchMesh->SetMaterial(0, BaseMaterial);
	}
}

// 부딪힐 때 사운드 + 따로 함수 만들어서 on/off 소리 다르게 하기
	/*AISeeMeCharacter* OverlappingCharacter = Cast<AISeeMeCharacter>(OtherActor);
	if (AudioComponent && OverlappingCharacter)
	{
		if (OverlappingCharacter->AudioComponent == nullptr)
			return;

		if (HasAuthority())
		{
			APlayerController* PC = Cast<APlayerController>(OverlappingCharacter->GetController());
			if (AISMPlayerController* MyPC = Cast<AISMPlayerController>(PC)) // 우리가 만든 컨트롤러 클래스로 캐스팅
			{
				// 3. 그 특정 플레이어 컨트롤러에게만 소리를 재생하라고 RPC 명령을 내립니다.
				MyPC->ClientPlayLocalSound(AudioComponent->Sound); // SoundToPlay는 재생할 사운드 애셋
			}
		}
	}*/

