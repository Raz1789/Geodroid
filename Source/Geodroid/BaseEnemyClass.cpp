// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemyClass.h"

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
	CollisionComponent->bGenerateOverlapEvents = true;
	CollisionComponent->bEditableWhenInherited = true;
	CollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->SetSimulatePhysics(true);

	/***************** Setting the Component Collision function **********/
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABaseEnemyClass::OnHit);
}

// Called when the game starts or when spawned
void ABaseEnemyClass::BeginPlay()
{
	Super::BeginPlay();
	Pathfinder = NewObject<UA_Pathfinding>();
	CurrentNode = UMapClass::WorldToMapNode(GetActorLocation());
	IsEnemyDead = false;
	MaxEnemyHealth = 15.f;
	EnemyHealth = MaxEnemyHealth;
	EnemyVelocity = 100.f;
	UpdatePathList();
	DeathTimer.MaxTime = 1.f;
	DeathTimer.CurrentTime = 0.0f;
}

// Called every frame
void ABaseEnemyClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsEnemyDead)
	{
		MovePawnAlongPathList(DeltaTime);
	}
	else
	{
		DeathSequence(DeltaTime);
	}
	
}

void ABaseEnemyClass::ApplyDamage(float Damage)
{
	EnemyHealth = std::max(0.0f, EnemyHealth - Damage);
	if (EnemyHealth <= 0 && !IsEnemyDead)
	{
		IsEnemyDead = true;
		SendResourceToPlayer();
	}
}

void ABaseEnemyClass::UpdatePathList()
{
	//Update the PathTArray using the Get Path Function
	PathList = Pathfinder->CalculatePath(CurrentNode.NodeIndex, UMapClass::GetTargetNode().NodeIndex);

	//Update the PathCounter to End of the PathTArray.
	PathCounter = PathList.Num() - 2; /// -2 since the -1 is Current Node and Next Node is -2
}

float ABaseEnemyClass::GetHealth()
{
	return EnemyHealth / MaxEnemyHealth;
}

void ABaseEnemyClass::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (Cast<AGeodroidProjectile>(OtherActor))
	{
		ApplyDamage(AGeodroidProjectile::GetBulletDamage());
		UE_LOG(LogTemp, Warning, TEXT("EnemyHealth: %f"), GetHealth());
	}
}

void ABaseEnemyClass::MovePawnAlongPathList(float DeltaTime)
{
	FVector EnemyPosition = GetActorLocation();
	/// Get Enemy node from the Map nodes
	CurrentNode = UMapClass::WorldToMapNode(EnemyPosition);

	if (PathCounter >= 0)
	{
		///Get the next Target Node Position in World
		FVector TargetPosition = UMapClass::GetMapNodePosition(PathList[PathCounter].X, PathList[PathCounter].Y);

		///Make EnemyPosition.Z and TargetPosition.Z same to eleminate the distance error caused will calculating distance
		EnemyPosition.Z = TargetPosition.Z;

		///Get Normalized Vector to Target
		FVector TargetVector = -EnemyPosition + TargetPosition;
		TargetVector.Normalize();

		///Calculate the distance to next node
		float TargetDistance = FVector::DistSquared(TargetPosition, EnemyPosition);

		if (TargetDistance > 10000.f) /// 10000 => 100 distance squared
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
			AddMovementInput(GetActorForwardVector(), 0.5f);
		}
		else
		{
			///If Target Reached, Point Counter to next Target on PathList
			PathCounter--;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Entered: %s"), *CurrentNode.NodeIndex.ToString());
		if (UPointerProtection::CheckAndLog(GetWorld(), "World Pointer"))
		{
			GetWorld()->DestroyActor(this);
		}
	}
}

void ABaseEnemyClass::SendResourceToPlayer()
{
	if (UPointerProtection::CheckAndLog(GetWorld(), "World"))
	{
		if (UPointerProtection::CheckAndLog(Cast<AGeodroidCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()), "Player Actor"))
		{
			AGeodroidCharacter* Player = Cast<AGeodroidCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
			Player->AddGold(EnemyGold);
		}
	}

}

void ABaseEnemyClass::DeathSequence(float DeltaTime)
{
	DeathTimer.CurrentTime += DeltaTime;
	if (DeathTimer.CurrentTime > DeathTimer.MaxTime)
	{
		if (UPointerProtection::CheckAndLog(GetWorld(), "World Pointer"))
		{
			GetWorld()->DestroyActor(this);
		}
	}
	else
	{
		SetActorHiddenInGame(std::cos((180 * DeathTimer.CurrentTime) / (3.14 * 3)) > 0);
	}
}

