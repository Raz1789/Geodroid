// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemyClass.h"

// UNREAL HEADER FILES
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"

// PROJECT HEADER FILES
#include "PointerProtection.h"
#include "A_Pathfinding.h"
#include "GeodroidProjectile.h"
#include "GeodroidCharacter.h"

///***********************************************************************************************************///
///                                               CONSTRUCTOR
///***********************************************************************************************************////
// Sets default values
ABaseEnemyClass::ABaseEnemyClass()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	

	/************** Setting the Collision component *********************/
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CapsuleComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetRelativeScale3D(FVector(1.f, 1.f, 2.f));
	CollisionComponent->SetSphereRadius(45.f);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->bEditableWhenInherited = true;
	CollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->SetSimulatePhysics(true);

	/***************** Setting the Component Collision function **********/
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABaseEnemyClass::OnHit);

	/***************** Setting Default Values*****************************/
	CurrentState = EnemyState::FollowPath;
	PreviousState = CurrentState;
	EnemyVelocity = 600.f;
	EnemyGold = 20;
	MaxEnemyHealth = 15.f;
	EnemyFireRate = 3.f;
	AttackDamage = 5.f;
	SpeedImpact = 1.f;
	bReachedEndPointSuccessfully = false;
	bGameModeRetrievedInformation = false;
}

///***********************************************************************************************************///
///                                             BEGIN PLAY FUNCTION
///***********************************************************************************************************///
// Called when the game starts or when spawned
void ABaseEnemyClass::BeginPlay()
{
	Super::BeginPlay();

	if (UPointerProtection::CheckAndLog(
		GetWorld(),
		"Enemy World Pointer")) //Check if Get World is not null and display error message
	{
		World = GetWorld(); 

		if (UPointerProtection::CheckAndLog(
			Cast<AGeodroidCharacter>(World->GetFirstPlayerController()->GetPawn()),
			"Enemy Player Pointer"))	//Check if PlayerPawn is not null and display error message
		{
			Player = Cast<AGeodroidCharacter>(World->GetFirstPlayerController()->GetPawn());
		}
	}

	///Creating a PathList
	Pathfinder = NewObject<UA_Pathfinding>();
	UpdatePathList();
	CurrentNode = UMapClass::WorldToMapNode(GetActorLocation()); //Get Current Node from Map

	///Setting other game design variables to defaults;
	EnemyHealth = MaxEnemyHealth;
	DeathTimer.MaxTime = 1.f;
	DeathTimer.CurrentTime = 0.0f;
	TimeFromLastFire = EnemyFireRate;
}

///***********************************************************************************************************///
///                                      TICK FUNCTION
///***********************************************************************************************************///
// Called every frame
void ABaseEnemyClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Pointer Protection
	if (!Player) return;

	///Updating the CurrentState of the Enemy based on player visibility
	if (IsPlayerInVisibleRange() && CurrentState == EnemyState::FollowPath)
	{
		CurrentState = EnemyState::FollowPlayer;
	}
	else if(!IsPlayerInVisibleRange() && CurrentState == EnemyState::FollowPlayer)
	{
		CurrentState = EnemyState::FollowPath;
	}
	
	///Calculates the square distance to the end target
	float EndDistance = FVector::DistSquared(
		this->GetActorLocation(),
		UMapClass::GetMapNodePosition(
				UMapClass::GetTargetNode().NodeIndex.X,
				UMapClass::GetTargetNode().NodeIndex.Y)
		);

	//Check if the enemy has reached the end target
	CheckTargetReached(EndDistance);

	///Execute the CurrentState Logic
	switch (CurrentState)
	{
	case EnemyState::FollowPath:

		///Update path list if the path was deviated in other State
		if (CurrentState != PreviousState)
		{
			UpdatePathList();
		}

		MovePawnAlongPathList(DeltaTime);

		break;

	case EnemyState::FollowPlayer:

		FollowPlayer(EndDistance, DeltaTime);

		break;

	case EnemyState::Dead:
		DeathSequence(DeltaTime);
		break;
	}

	//Update current state to previous state before end of tick
	PreviousState = CurrentState;

}

///***********************************************************************************************************///
///                                  CHECK IF TARGET REACHED
///***********************************************************************************************************///
void ABaseEnemyClass::CheckTargetReached(float EndDistance)
{
	//CHECK IF CURRENT NODE IS THE MAP TARGET NODE OR IS CLOSE TO THE TARGET
	if (CurrentNode == UMapClass::GetTargetNode() || EndDistance < 40000.f) //40000 => 200^2 SINCE IN SQUARE DISTANCE
	{
		CurrentState = EnemyState::Dead;

		//SET END REACHED FLAG FOR GAME MODE FLAME HEALTH DEDUCTION
		if (!bGameModeRetrievedInformation)
			bReachedEndPointSuccessfully = true;
	}
}

///***********************************************************************************************************///
///                                  MOVE THE TARGET ALONG THE PATH LIST
///***********************************************************************************************************///
void ABaseEnemyClass::MovePawnAlongPathList(float DeltaTime)
{
	//Pointer protection
	if (!World) return;

	//The enemyposition in Worldspace
	FVector EnemyPosition = GetActorLocation();

	/// Get Enemy node from the Map nodes
	CurrentNode = UMapClass::WorldToMapNode(EnemyPosition);

	///Check if not end of pathlist
	if (PathCounter >= 0)
	{
		///Get the next Target Node Position in World
		FVector TargetPosition = UMapClass::GetMapNodePosition(PathList[PathCounter].X, PathList[PathCounter].Y);

		///Make EnemyPosition.Z and TargetPosition.Z same to eleminate the distance
		///serror caused will calculating distance
		EnemyPosition.Z = TargetPosition.Z;

		///Get Normalized Vector to Target
		FVector TargetVector = -EnemyPosition + TargetPosition;
		TargetVector.Normalize();

		///Calculate the distance to next node
		float TargetDistance = FVector::DistSquared(TargetPosition, EnemyPosition);

		if (TargetDistance >  40000.f) /// 10000 => 100 distance squared
		{
			MoveToTargetVector(TargetVector);
		}
		else
		{
			///If Target Reached, Point Counter to next Target on PathList
			PathCounter--;
		}
	}
	else //end of pathlist is always the end node
	{

		CurrentState = EnemyState::Dead;

		//SET END REACHED FLAG FOR GAME MODE FLAME HEALTH DEDUCTION
		if (!bGameModeRetrievedInformation)
			bReachedEndPointSuccessfully = true;
	}
}

///***********************************************************************************************************///
///                                  FOLLOW THE PLAYER AROUND TILL VISIBLE IN A RANGE
///***********************************************************************************************************///
void ABaseEnemyClass::FollowPlayer(float EndDistance, float DeltaTime)
{
	/**************************** MOVEMENT UPDATE *************************************************/

	//Calculated the distance of the enemy from the player
	FVector TargetVector = Player->GetActorLocation() - this->GetActorLocation();
	TargetVector.Normalize();
	float TargetDistance = FVector::DistSquared(Player->GetActorLocation(), this->GetActorLocation());

	//check if the end target reached or move to player
	if (EndDistance >  40000.f && TargetDistance >= 80000.f)
	{
		MoveToTargetVector(TargetVector);
	}
	else
	{
		//Check if the enemy has reached the end target
		CheckTargetReached(EndDistance);
	}



	/**************************** SHOOTING UPDATE ******************************************/
	///Updating the Firetimer
	TimeFromLastFire += DeltaTime;

	///Clipping at random max prevent float overflow
	if (TimeFromLastFire > EnemyFireRate * 1000.f)
	{
		TimeFromLastFire = EnemyFireRate + 1.f;
	}

	//check and shoot at player
	ShootAtPlayer();
}

///***********************************************************************************************************///
///                            MOVEMENT FUNCTION FOR THE ENEMY PAWN
///***********************************************************************************************************///
void ABaseEnemyClass::MoveToTargetVector(const FVector & TargetVector)
{
	///Calculate the direction of target
	FRotator LookRotator = TargetVector.Rotation() - GetActorForwardVector().Rotation();
	///Correct the pitch
	LookRotator.Pitch = 0.0f;
	///Get the Rotator with respect to current rotation
	LookRotator += GetActorRotation();
	///Rotate head towards the target
	SetActorRotation(FMath::Lerp(GetActorRotation(), LookRotator, 0.15f));

	///move towards target
	AddMovementInput(GetActorForwardVector(), (EnemyVelocity / 1200.f) * SpeedImpact);
}

///***********************************************************************************************************///
///                            SHOOT FUNCTION FOR THE ENEMY
///***********************************************************************************************************///
void ABaseEnemyClass::ShootAtPlayer()
{
	//Pointer Protection
	if (!World) return;

	//Checking if Enemy can fire
	if (TimeFromLastFire > EnemyFireRate)
	{
		//Reset timer
		TimeFromLastFire = 0.f;

		// try and fire a projectile
		if (UPointerProtection::CheckAndLog(ProjectileClass, "Enemy Projectile"))
		{

			AGeodroidProjectile* Projectile;

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = 
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			ActorSpawnParams.Instigator = this;

			// spawn the projectile at the muzzle
			Projectile = World->SpawnActor<AGeodroidProjectile>(ProjectileClass,
				(GetActorLocation() + (GetActorForwardVector() * 100.f)),
																this->GetActorRotation(), ActorSpawnParams);

			//sets the bullet damage
			if (UPointerProtection::CheckAndLog(Projectile, "Enemy Projectile"))
			{
				Projectile->SetBulletDamage(AttackDamage);
			}
		}

		// try and play the sound if specified
		if (UPointerProtection::CheckAndLog(FireSound, "Enemy Fire Sound"))
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

///***********************************************************************************************************///
///                        UPDATES THE PATH TO THE END NODE FROM GAME MODE
///***********************************************************************************************************///
void ABaseEnemyClass::UpdatePathList(FVector2D TargetNodeIndex)
{
	Pathfinder = NewObject<UA_Pathfinding>();

	PathList.Empty();

	//Update the PathTArray using the Get Path Function
	PathList.Append(Pathfinder->GetPathList(CurrentNode.NodeIndex));

	//Update the PathCounter to End of the PathTArray.
	PathCounter = PathList.Num() - 2; /// -2 since the -1 is Current Node and Next Node is -2
}

///***********************************************************************************************************///
///                          PLAY THE DEATH SEQUENCE
///***********************************************************************************************************///
void ABaseEnemyClass::DeathSequence(float DeltaTime)
{
	//Pointer Protection
	if (!World) return;

	DeathTimer.CurrentTime += DeltaTime;

	if (DeathTimer.CurrentTime > DeathTimer.MaxTime) //CHECK IF DEATH TIMEER EXPIRED
	{
		World->DestroyActor(this);
	}
	else
	{
		//THE BLINKING EFFECT
		SetActorHiddenInGame(UKismetMathLibrary::Cos((180 * DeathTimer.CurrentTime) / (3.14 * 3)) > 0);
	}
}

///***********************************************************************************************************///
///                          CHECK IF THE PLAYER IS VISIBLE TO THE ENEMY
///***********************************************************************************************************///
bool ABaseEnemyClass::IsPlayerInVisibleRange()
{
	//Pointer Protection
	if (!World) return false;

	FHitResult OutHit;

	///Getting the Direction to player
	FVector DirectionVectorToPlayer;
	DirectionVectorToPlayer = Player->GetActorLocation() - this->GetActorLocation();
	DirectionVectorToPlayer.Normalize();

	///Setting the CollisionQueryParams
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	//LINE TRACE ON THE VISIBILITY CHANNEL
	World->LineTraceSingleByChannel(OutHit,
									GetActorLocation(),
									GetActorLocation() + DirectionVectorToPlayer * (UMapClass::GetWorldNodeSize() * 2.f),
									ECC_Visibility,
									CollisionParam);

	//Check if the actor hit is player
	if (OutHit.GetActor() == Player)
	{
		return true;
	}

	return false;
}

///***********************************************************************************************************///
///                          EVENT DRIVEN FUNCTION TO SEE IF PROJECTILE HAS HIT
///***********************************************************************************************************///
void ABaseEnemyClass::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	AGeodroidProjectile* Collider = Cast<AGeodroidProjectile>(OtherActor);
	ABaseEnemyClass* ColliderInstigator = Cast<ABaseEnemyClass>(OtherActor->GetInstigator());

	if (Collider && !ColliderInstigator)
	{
		ApplyDamage(Collider->GetBulletDamage());
	}
}

///***********************************************************************************************************///
///                          APPLY THE DAMAGE ON ENEMY
///***********************************************************************************************************///
void ABaseEnemyClass::ApplyDamage(float Damage)
{
	EnemyHealth = std::max(0.0f, EnemyHealth - Damage);
	if (EnemyHealth <= 0 && CurrentState != EnemyState::Dead)
	{
		CurrentState = EnemyState::Dead;
		SendResourceToPlayer();
	}
}

///***********************************************************************************************************///
///                   SEND THE ENEMYGOLD TO PLAYER
///***********************************************************************************************************///
void ABaseEnemyClass::SendResourceToPlayer()
{
	//Pointer Protection
	if (!Player) return;

	Player->AddGold(EnemyGold);

}

///***********************************************************************************************************///
///                   TRIGGERED BY SLOW TRAP SETTING THE VELOCITY IMPACT FACTOR TO 0.5
///***********************************************************************************************************///
void ABaseEnemyClass::AddSpeedEffect()
{
	SpeedImpact = 0.5f;
}

///***********************************************************************************************************///
///                   TRIGGERED BY SLOW TRAP RESETS IMPACT FACTOR ON EXIT
///***********************************************************************************************************///
void ABaseEnemyClass::ResetSpeedEffect()
{
	SpeedImpact = 1.f;
}

///***********************************************************************************************************///
///                   RETURNS THE HEALTH OF THIS ENEMY
///***********************************************************************************************************///
float ABaseEnemyClass::GetHealth()
{
	return EnemyHealth / MaxEnemyHealth;
}

///***********************************************************************************************************///
///                   SET TO TRUE WHEN ENEMY REACHES THE ETERNAL FLAME
///***********************************************************************************************************///
bool ABaseEnemyClass::HasEnemyReachedEndpoint()
{
	return bReachedEndPointSuccessfully;
}

///***********************************************************************************************************///
///     CALLED FROM GAME MODE TO RESET THE VALUES SO AS TO NOTIFY THAT THE VALUE HAS BEEN RECEIVED
///		TWO COMMUNICATION RADIO :P
///***********************************************************************************************************///
void ABaseEnemyClass::ResetReachedEndPointSuccessfully()
{
	bReachedEndPointSuccessfully = false;
	bGameModeRetrievedInformation = true;
}



