// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"

ATurret::ATurret()
{
	///Default Initialization of Varaibles
	BP_BuildCost = 100;
	bIsNodeWalkable = false;
	AttackRate = 1.f;
	AttackDamage = 5.f;
	CurrentLevelOfStructure = ELevel::Level1;
	BuildLevelLimit = ELevel::Level2;

	///Turret Influence Components
	InfluenceCircle = FCollisionShape::MakeSphere(UMapClass::GetWorldNodeSize() * 2 * 1.4f);

	RestrictionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("RestrictionArea"));
	RestrictionArea->SetBoxExtent(FVector(200.f));
	RestrictionArea->BodyInstance.SetCollisionProfileName("BlockAll");

	RootComponent = RestrictionArea;

	///Turret Color
	MaterialColor = FColor(0.f, 1.f, 0.f, 0.25f);

}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
	///Default Initialization of Varaibles from Blueprint
	BuildCost = BP_BuildCost;

	if (UPointerProtection::CheckAndLog(GetWorld(), "World"))
	{
		World = GetWorld();
	}

	ActorLocation = GetActorLocation();

	if (!World) return;
	
	DrawDebugSphere(World, GetActorLocation(), InfluenceCircle.GetSphereRadius(), 25, FColor::Magenta, true);

}

void ATurret::CheckAndExecuteAttack(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{}


void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!World) return;

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel3);
	if (World->SweepMultiByObjectType(OutHits, ActorLocation, ActorLocation, FQuat::Identity, ObjectParams, InfluenceCircle))
	{
		bool bIsTargetActorPresentInHitResult = false;
		if (TargetActor)
		{
			for (FHitResult& Hit : OutHits)
			{
				if (Hit.GetActor() == TargetActor)
				{
					bIsTargetActorPresentInHitResult = true;
					break;
				}
			}
			if (!bIsTargetActorPresentInHitResult)
			{
				TargetActor = OutHits[0].GetActor();
			}
		}
		else
		{
			TargetActor = OutHits[0].GetActor();
		}
		ReceiveLookAtTargetEnemy(TargetActor);
	}
	
	
}
