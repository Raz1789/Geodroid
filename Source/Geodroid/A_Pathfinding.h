// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "MapClass.h"
#include "A_Pathfinding.generated.h"

UCLASS()
class UA_Pathfinding : public UObject
{
	GENERATED_BODY()

private:

	//Size of the GameMode Map
	FVector2D MapMaxSize;

	//A Linked List
	struct PathNode
	{
		///MEMBER VARIABLE
		FVector2D CurrentMapNode;
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

	//List of index subtractors for finding the neighbours
	TArray<FVector2D> NeighbourList;

public:

	//Constructor for UA_Pathfinding
	UA_Pathfinding();

	//Calculate the Path
	TArray<FVector2D> GetPathList(FVector2D StartIndex, FVector2D EndIndex = UMapClass::GetTargetNode().NodeIndex);

	bool CalculatePathList(TArray<FVector2D>& OutPathList, const FVector2D& StartIndex, const FVector2D& EndIndex = UMapClass::GetTargetNode().NodeIndex, bool bIsThisACheckOnly = true);

	bool CheckBoundary(FVector2D &TempVector) const;

	//Make PathList from the PathNode LinkedList
	TArray<FVector2D> MakePathList(PathNode* FinalPathNode);

	bool PathExist(FVector2D StartIndex);

};