// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemyClass.h"

// Sets default values
ABaseEnemyClass::ABaseEnemyClass()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	

}

// Called when the game starts or when spawned
void ABaseEnemyClass::BeginPlay()
{
	Super::BeginPlay();
	Pathfinder = NewObject<UA_Pathfinding>();
	CurrentNode = UMapClass::WorldToMapNode(GetActorLocation());
	IsEnemyDead = false;
	PreviousNode = CurrentNode;
	MaxEnemyHealth = 15.f;
	EnemyHealth = MaxEnemyHealth;
	EnemyVelocity = 100.f;
	UpdatePathList();
	
	DeathTimer.MaxTime = 2.0f;
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
		DeathTimer.CurrentTime += DeltaTime;
		if (DeathTimer.CurrentTime > DeathTimer.MaxTime)
		{
			Destroy();
		}
		else
		{
			SetActorHiddenInGame(std::cos((180 * DeathTimer.CurrentTime) / (3.14 * 3)) > 0);
		}
	}
}

void ABaseEnemyClass::ApplyDamage(float Damage)
{
	EnemyHealth = std::max(0.0f, EnemyHealth - Damage);
	if (EnemyHealth <= 0)
	{
		IsEnemyDead = true;
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
	return EnemyHealth / (MaxEnemyHealth * 2);
}

void ABaseEnemyClass::MovePawnAlongPathList(float DeltaTime)
{
	FVector EnemyPosition = GetActorLocation();
	CurrentNode = UMapClass::WorldToMapNode(EnemyPosition);
	if (PathCounter >= 0)
	{
		///Get the currentNodea
		FVector TargetPosition = UMapClass::GetMapNodePosition(PathList[PathCounter].X, PathList[PathCounter].Y);
		FVector TargetVector = -EnemyPosition + TargetPosition;
		TargetVector.Z = EnemyPosition.Z;
		TargetVector.Normalize();
		float DistanceToTarget = FVector::DistSquared(TargetPosition, EnemyPosition);
		///Check if the Distance to TargetNode is less than 102
		if (DistanceToTarget > (2500.f)) ///Since Distance squared used
		{
			///Calculate the direction of target
			FRotator LookRotator = TargetVector.Rotation() - GetActorForwardVector().Rotation();
			///Correct the pitch
			LookRotator.Pitch = 0.0f;
			///Get the Rotator with respect to current rotation
			LookRotator += GetActorRotation();
			///Rotate head towards the target
			SetActorRotation(LookRotator);
			///move towards target
			//AddMovementInput(LookRotator.Vector(), 0.1f);
			//UE_LOG(LogTemp, Warning, TEXT("CurrentPosition: %s"), *TargetVector.ToString());
			//UE_LOG(LogTemp, Warning, TEXT("LookRotation: %s"), *LookRotator.ToString());
			//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), DistanceToTarget);
			//UE_LOG(LogTemp, Warning, TEXT("PathCounter: %s"), *PathList[PathCounter].ToString());

			//FVector NewLocation;
			//NewLocation = GetActorLocation() + (GetActorForwardVector() * (EnemyVelocity * DeltaTime));
			//SetActorLocation(NewLocation);

			AddMovementInput(GetActorForwardVector(), 0.5f);
		}
		else
		{
			PathCounter--;
		}

		PreviousNode = CurrentNode;
	}
	else if (CurrentNode == UMapClass::GetTargetNode())
	{
		UE_LOG(LogTemp, Warning, TEXT("Entered: %s"), *CurrentNode.NodeIndex.ToString());
		Destroy();
	}
}

