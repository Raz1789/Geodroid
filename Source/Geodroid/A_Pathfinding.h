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
		///MEMBER VARIABLE
		FVector2D CurrentPathNode;
		PathNode* PreviousPathNode;
		float GCost;
		float HCost;
		float FCost;

		///CONSTRUCTOR
		PathNode(FVector2D _CurrentPathNode, PathNode* _PreviousPathNode, FVector2D _StartPathNode, FVector2D _TargetPathNode);

		///MEMBER FUNCTIONS
		//Operator "<" overloading for Sort operation
		bool operator< (const PathNode& other) const;

		//Operator "==" overloading for Contains operation
		bool operator== (const PathNode& other) const;

		void CalculateCost(FVector2D StartPathNode, FVector2D TargetPathNode);

		///DESTRUCTOR
		~PathNode();
	};

	//Open List variable created for speed optimization
	TArray<PathNode*> OpenedList;

	//Closed List variable created for speed optimization
	TArray<PathNode*> ClosedList;

	//Open List variable created for speed optimization
	TArray<FVector2D> OutputList;

	//List of index subtractors for finding the neighbours
	TArray<FVector2D> NeighbourList;

public:

	//Calculate the Path
	TArray<FVector2D> CalculatePath(FVector2D StartIndex, FVector2D EndIndex);

	bool CheckBoundary(FVector2D &TempVector) const;

	void MakePathList(PathNode* FinalPathNode);

};