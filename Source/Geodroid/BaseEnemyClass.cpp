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
	UE_LOG(LogTemp, Error, TEXT("EnemyNode: %s"), *CurrentNode.NodeIndex.ToString());
	PathList = Pathfinder->CalculatePath(CurrentNode.NodeIndex, UMapClass::GetTargetNode().NodeIndex);
	for (int32 PathCounter = 0; PathCounter < PathList.Num(); PathCounter++)
	{
		UE_LOG(LogTemp, Warning, TEXT("PathList: %s"), *PathList[PathCounter].ToString());
	}
}

// Called every frame
void ABaseEnemyClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Check if the MapNodeStatus has Changed
		//Update the PathTArray using the Get Path Function
		//Update the PathCounter to End of the PathTArray.

	//Loop till the PathCounter reaches zero
		//Get the pawn's current pathnode.
		//if current node is not previous node
			//Increment the PathCounter
		//Call MoveToNextNode function with the TargetNode
		

}

