// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"

///***********************************************************************************************************///
///                                               CONSTRUCTOR
///***********************************************************************************************************////
ATurret::ATurret()
{
	///Initiate ticking
	PrimaryActorTick.bCanEverTick = true;

	///Default Initialization of Varaibles
	BP_BuildCost = 150;
	bIsStructureWalkable = false;
	bHasSweepHitAnyObject = false;
	AttackRate = 1.f;
	AttackDamage = 5.f;

	///Turret Influence Components
	InfluenceCircleRadius = UMapClass::GetWorldNodeSize() * 2 * 1.4f;
	InfluenceSphere = FCollisionShape::MakeSphere(InfluenceCircleRadius);

	///CollisionComponent
	RestrictionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("RestrictionArea"));
	RestrictionArea->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	RestrictionArea->SetBoxExtent(FVector(200.f, 200.f, 25.f));
	RestrictionArea->BodyInstance.SetCollisionProfileName("BlockAll");
	RootComponent = RestrictionArea;
}

///***********************************************************************************************************///
///                                             BEGIN PLAY FUNCTION
///***********************************************************************************************************///
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	///Set Ticking true
	PrimaryActorTick.bCanEverTick = true;

	///Setting World Variable
	if (UPointerProtection::CheckAndLog(GetWorld(), "World"))
	{
		World = GetWorld();
	}

	///Setting the Actor Location
	ActorLocation = GetActorLocation();

}

///***********************************************************************************************************///
///                                               TICK FUNCTION
///***********************************************************************************************************///
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	///Checking if the structure was activated
	if (bIsStructureActive)
	{
		///Updating the LastFiretimer
		TimeFromLastFire += DeltaTime;

		///Search the Influence Circle for Enemies
		SearchForEnemy();

		if (bHasSweepHitAnyObject) ///check if sweep detected any objects
		{
			///Blueprint Implemented Function to point the Turret at Target Enemy
			ReceiveLookAtTargetEnemy(TargetActor);

			///Visibility check
			bool bIsEnemyVisible = IsEnemyInVisibleRange(TargetActor);

			if (bIsEnemyVisible)
			{
				ShootAtEnemy(TargetActor); ///Shoot at Enemy
			}
		}

		///Clipping at random max prevent float overflow
		if (TimeFromLastFire > AttackRate * 1000.f)
		{
			TimeFromLastFire = AttackRate + 1.f;
		}

	}
}

///***********************************************************************************************************///
///                                              SEARCH FOR ENEMY
///***********************************************************************************************************///
void ATurret::SearchForEnemy()
{
	//Pointer Protection
	if (!World) return;

	///----------- Checking for enemies inside of the influence circle -------------------------------///
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel3); ///ECC_GameTraceChannel3 => Enemy Trace Channel

	///Sweep for Multiple enemies in Enemy Trace Channel
	bHasSweepHitAnyObject = World->SweepMultiByObjectType(OutHits,
														  ActorLocation,
														  ActorLocation,
														  FQuat::Identity,
														  ObjectParams,
														  InfluenceSphere);
	///check if Sweep Hit an Enemy
	if (bHasSweepHitAnyObject)
	{
		/// variable to check if the previous Target is still inside the Influence Circle
		bool bIsTargetActorPresentInHitResult = false; ///default value to false

		if (TargetActor) /// pointer check for Target
		{
			/// Iterate over all enemies inside the Influence Circle 
			/// and search for the previously shooting enemy
			for (FHitResult& Hit : OutHits)
			{
				if (Hit.GetActor() == TargetActor)
				{
					bIsTargetActorPresentInHitResult = true;
					break;
				}
			}
			/// If not found set to first Enemy inside Influence Circle
			if (!bIsTargetActorPresentInHitResult)
			{
				TargetActor = OutHits[0].GetActor();
			}
		}
		else
		{
			/// Target not set hence setting to
			/// first Enemy inside Influence Circle
			TargetActor = OutHits[0].GetActor();
		}
	}

}

///***********************************************************************************************************///
///                                           CHECK IF ENEMY IS VISIBLE
///***********************************************************************************************************///
bool ATurret::IsEnemyInVisibleRange(const  AActor* _TargetActor)
{
	//Pointer Protection
	if (!World) return false;
	if (!BP_Turret) return false;

	///Getting the spawn Location
	FVector StartLocation;

	///Vector to Enemy = -TurretVector from origin + EnemyVector from Origin (Shoot Offset is not required for visibility check)
	StartLocation = -BP_Turret->GetComponentLocation() + _TargetActor->GetActorLocation();
	StartLocation.Normalize(); ///To get the Direction Vector (Unit magnitude)
	FVector SpawnDirection = StartLocation;
	StartLocation *= 100.f; ///Scale to get a point 100cm from the start of vector (Turret Barrel length)
	StartLocation += BP_Turret->GetComponentLocation(); ///Translating the Vector to the Turret

	///Setting the CollisionQueryParams
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	FHitResult OutHit; ///Receive the first Target hit

	///LineTrace in the Visibility channel
	World->LineTraceSingleByChannel(OutHit,
									StartLocation,
									StartLocation + SpawnDirection * (InfluenceSphere.GetSphereRadius()),
									ECC_Visibility,
									CollisionParam);

	///Check if the actor Hit is equal to the enemy found
	if (OutHit.GetActor() == _TargetActor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

///***********************************************************************************************************///
///                                               SHOOT AT ENEMY
///***********************************************************************************************************///
void ATurret::ShootAtEnemy(const AActor* TargetActor)
{
	///Pointer check
	if (UPointerProtection::CheckAndLog(BP_Turret, "Turret"))
	{
		///Is it time to shoot
		if (TimeFromLastFire > AttackRate)
		{
			///Reset Fire Timer
			TimeFromLastFire = 0.f;

			/// Pointer check
			if (UPointerProtection::CheckAndLog(ProjectileClass, "Turret ProjectileClass"))
			{

				///Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				ActorSpawnParams.Instigator = Instigator;

				///Getting the Spawn Location
				FVector SpawnLocation;

				///Vector to Enemy = -TurretVector from origin + (EnemyVector + Origin + ShootDelay Offset)
				SpawnLocation = -BP_Turret->GetComponentLocation() + (TargetActor->GetActorLocation() + /*OFFSET*/(TargetActor->GetActorForwardVector() * 50.f));
				SpawnLocation.Normalize(); ///To get the Direction Vector
				FVector SpawnDirection = SpawnLocation;
				SpawnLocation *= 100.f; ///Scale to get a point 100cm from the start of vector (Turret Barrel length)
				SpawnLocation += BP_Turret->GetComponentLocation(); ///Translating the Vector to the Turret

				///Getting SpawnRotation
				FRotator SpawnRotation;
				SpawnRotation = SpawnDirection.Rotation();


				AGeodroidProjectile* Projectile;

				/// spawn the projectile at the Barrel tip
				Projectile = World->SpawnActor<AGeodroidProjectile>(ProjectileClass,
																	SpawnLocation,
																	SpawnRotation,
																	ActorSpawnParams);

				///Pointer Check
				if (UPointerProtection::CheckAndLog(Projectile, "Turret Projectile"))
				{
					Projectile->SetBulletDamage(AttackDamage); ///Apply the projectile damage
				}
			}

			///Pointer check
			if (UPointerProtection::CheckAndLog(FireSound, "Enemy Fire Sound"))
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation()); ///Play the sound
			}
		}
	}
}
