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
	RestrictionArea->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	RestrictionArea->SetBoxExtent(FVector(200.f,200.f, 50.f));
	RestrictionArea->BodyInstance.SetCollisionProfileName("BlockAll");

	RootComponent = RestrictionArea;

	///Turret Color
	MaterialColor = FColor(1.f, 0.f, 0.f, 0.25f);

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

void ATurret::CheckAndExecuteAttack()
{
	//Pointer Protection
	if (!World) return;

	///******************** SETTING TURRET ROTATION *************************///
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
		ShootAtEnemy(TargetActor);
	}
	
}

void ATurret::ShootAtEnemy(const AActor* TargetActor)
{
	if (UPointerProtection::CheckAndLog(BP_Turret, "Turret"))
	{
		if (TimeFromLastFire > AttackRate)
		{
			TimeFromLastFire = 0.f;
			// try and fire a projectile
			if (UPointerProtection::CheckAndLog(ProjectileClass, "Turret Projectile"))
			{
				AGeodroidProjectile* Projectile;
				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				///Getting the spawn Location
				FVector SpawnLocation;

				///Vector to Enemy = -TurretVector from origin + (EnemyVector + Origin + ShootDelay Offset)
				SpawnLocation = -BP_Turret->GetComponentLocation() + (TargetActor->GetActorLocation() + /*OFFSET*/(TargetActor->GetActorForwardVector() * 100.f));
				SpawnLocation.Normalize(); ///To get the Direction Vector
				FVector SpawnDirection = SpawnLocation;
				SpawnLocation *= 50.f; ///Scale to get a point 50cm from the start of vector
				SpawnLocation += BP_Turret->GetComponentLocation(); ///Traslating the Vector to the Turret

				///Getting SpawnRotation
				FRotator SpawnRotation;
				SpawnRotation = SpawnDirection.Rotation();

				AGeodroidProjectile::BulletDamage = 5.0f;

				// spawn the projectile at the muzzle
				Projectile = World->SpawnActor<AGeodroidProjectile>(ProjectileClass,
																	SpawnLocation,
																	SpawnRotation,
																	ActorSpawnParams);
			}

			// try and play the sound if specified
			if (UPointerProtection::CheckAndLog(FireSound, "Enemy Fire Sound"))
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}
		}
	}
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	///Updating the Firetimer
	TimeFromLastFire += DeltaTime;

	///Clipping at random max prevent float overflow
	if (TimeFromLastFire > AttackRate * 1000.f)
	{
		TimeFromLastFire = AttackRate + 1.f;
	}	

	CheckAndExecuteAttack();
}
