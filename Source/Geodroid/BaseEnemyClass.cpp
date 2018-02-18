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
	PreviousNode = CurrentNode;
	EnemyVelocity = 500.f;

	UpdatePathList();
}

// Called every frame
void ABaseEnemyClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MovePawnAlongPathList(DeltaTime);
}

void ABaseEnemyClass::UpdatePathList()
{
	//Update the PathTArray using the Get Path Function
	PathList = Pathfinder->CalculatePath(CurrentNode.NodeIndex, UMapClass::GetTargetNode().NodeIndex);

	//Update the PathCounter to End of the PathTArray.
	PathCounter = PathList.Num() - 2; /// -2 since the -1 is Current Node and Next Node is -2
}

void ABaseEnemyClass::MovePawnAlongPathList(float DeltaTime)
{
	if (PathCounter >= 0)
	{
		///Get the currentNode
		CurrentNode = UMapClass::WorldToMapNode(GetActorLocation());
		FVector TargetVector = -GetActorLocation() + UMapClass::GetMapNodePosition(PathList[PathCounter].X, PathList[PathCounter].Y);
		///Check if	the CurrentNode is the Previousnode

		if (CurrentNode == PreviousNode)
		{
			///Calculate the direction of target
			FRotator LookRotator = TargetVector.Rotation() - GetActorForwardVector().Rotation();
			///Correct the pitch
			LookRotator.Pitch = 0.0f;
			///Get the Rotator with respect to current rotation
			LookRotator += GetActorRotation();
			///Rotate head towards the target
			SetActorRotation(LookRotator);
			UE_LOG(LogTemp, Warning, TEXT("Enemy Location: %s"), *CurrentNode.NodeIndex.ToString());
			///move towards target
			FVector NewLocation;
			NewLocation = GetActorLocation() + (GetActorForwardVector() * (EnemyVelocity * DeltaTime));
			SetActorLocation(NewLocation);
		}
		else
		{
			PathCounter--;
		}

		PreviousNode = CurrentNode;
	}
}

