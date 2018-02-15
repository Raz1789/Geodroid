// Fill out your copyright notice in the Description page of Project Settings.

#include "A_Pathfinding.h"

#pragma region STRUCT FUNCTION DEFINITIONS


bool UA_Pathfinding::PathNode::operator< (const PathNode& other) const
{
	if (FCost < other.FCost)
	{
		return true;
	}
	else if (FCost == other.FCost && HCost < other.HCost)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UA_Pathfinding::PathNode::CalculateCost(FVector2D StartPathNode, FVector2D TargetPathNode)
{

}


#pragma endregion

TArray<FVector2D> UA_Pathfinding::CalculatePath(FVector2D StartIndex, FVector2D EndIndex)
{
	//If MapNodeStatus has Changed or if this is the initial run of this function
	if (AGeodroidGameMode::IsMapNodeStatusChanged() || MapWalkableArray.Num() == 0)
	{
		//Make a copy of the MapWalkableArray in this class and get the MapSize
		MapWalkableArray = AGeodroidGameMode::GetMapWalkableArray();

		MapMaxSize = AGeodroidGameMode::GetMapMaxSize();
	}




	return TArray<FVector2D>();
}
