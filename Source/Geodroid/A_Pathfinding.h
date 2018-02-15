// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GeodroidGameMode.h"
#include "A_Pathfinding.generated.h"

UCLASS()
class UA_Pathfinding : public UObject
{
	GENERATED_BODY()
private:
	//Copy of the GameMode MapWalkableArray Variable
	TArray<bool> MapWalkableArray;

	//Size of the GameMode Map
	FVector2D MapMaxSize;

	//A Linked List
	struct PathNode
	{
		FVector2D CurrentPathNode;
		PathNode* PreviousPathNode;
		float GCost;
		float HCost;
		float FCost;

		///MEMBER FUNCTIONS
		//Operator "<" for sorting operation
		bool operator< (const PathNode& other) const;

		void CalculateCost(FVector2D StartPathNode, FVector2D TargetPathNode);
	};

public:

	//Calculate the Path
	TArray<FVector2D> CalculatePath(FVector2D StartIndex, FVector2D EndIndex);

};