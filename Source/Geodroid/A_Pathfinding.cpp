// Fill out your copyright notice in the Description page of Project Settings.

#include "A_Pathfinding.h"

#pragma region STRUCT FUNCTION DEFINITIONS

UA_Pathfinding::PathNode::PathNode(FVector2D _CurrentPathNode, PathNode* _PreviousPathNode, FVector2D _StartPathNode, FVector2D _TargetPathNode)
{
	CurrentPathNode = _CurrentPathNode;
	PreviousPathNode = _PreviousPathNode;
	CalculateCost(_StartPathNode, _TargetPathNode);
}

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

bool UA_Pathfinding::PathNode::operator== (const PathNode& other) const
{
	if (CurrentPathNode.X == other.CurrentPathNode.X && CurrentPathNode.Y == other.CurrentPathNode.Y)
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
	GCost = FVector2D::DistSquared(StartPathNode, CurrentPathNode);
	HCost = FVector2D::DistSquared(TargetPathNode, CurrentPathNode);
	FCost = GCost + HCost;
}

UA_Pathfinding::PathNode::~PathNode()
{
	delete PreviousPathNode;
}

#pragma endregion

#pragma region CLASS MEMBER FUNCTION DEFINITIONS

TArray<FVector2D> UA_Pathfinding::CalculatePath(FVector2D StartIndex,
	FVector2D TargetIndex)
{
	///Initializing NeighbourList only first time
	UE_LOG(LogTemp, Warning, TEXT("Entered 0: NeihbourList %d"), NeighbourList.Num());

	if (NeighbourList.Num() == 0)
	{
		NeighbourList.Add(FVector2D(-1, 0));
		NeighbourList.Add(FVector2D(0, -1));
		NeighbourList.Add(FVector2D(1, 0));
		NeighbourList.Add(FVector2D(0, 1));
	}
	
	///If MapNodeStatus has Changed or if this is the initial run of this function
	if (AGeodroidGameMode::IsMapNodeStatusChanged() || MapWalkableArray.Num() == 0)
	{
		///Make a copy of the MapWalkableArray in this class and get the MapSize
		MapWalkableArray = AGeodroidGameMode::GetMapWalkableArray();
		MapMaxSize = AGeodroidGameMode::GetMapMaxSize();
	}

	///Clearing the temporary variables
	OutputList.Empty();
	OpenedList.Empty();
	ClosedList.Empty();

	///Create a temporary PathNode for initial location
	PathNode* TempPathNode = new PathNode(StartIndex, nullptr, StartIndex, TargetIndex);

	///Temporary FVector2D for Speed Optimization
	FVector2D TempVector;

	///Assign it to OpenedList
	OpenedList.Add(TempPathNode);
	int32 LoopCounter = 0;

	///Loop Till OpenedList is not Empty
	while (OpenedList.Num() > 0 && LoopCounter < 10)
	{
		///Sort OpenedList, assign smallest to ClosedList and remove from OpenList
		OpenedList.Sort();
		ClosedList.Add(OpenedList[0]);
		OpenedList.RemoveAt(0);

		UE_LOG(LogTemp, Error, TEXT("--------------------------------------------"));

		for (int32 ClosedCounter = 0; ClosedCounter < ClosedList.Num(); ClosedCounter++)
		{
			UE_LOG(LogTemp, Warning, TEXT("ClosedCounter %d: %s"), ClosedCounter, *ClosedList[ClosedCounter]->CurrentPathNode.ToString());
		}

		UE_LOG(LogTemp, Error, TEXT("--------------------------------------------"));

		for (int32 OpenedCounter = 0; OpenedCounter < OpenedList.Num(); OpenedCounter++)
		{
			UE_LOG(LogTemp, Warning, TEXT("OpenedCounter %d: %s"), OpenedCounter, *OpenedList[OpenedCounter]->CurrentPathNode.ToString());
		}

		///Getting X and Y of the node that needs to be checked
		int32 X = ClosedList[ClosedList.Num() - 1]->CurrentPathNode.X;
		int32 Y = ClosedList[ClosedList.Num() - 1]->CurrentPathNode.Y;

		///Searching the Valid Neighbours for Least FCost
		for (int32 Counter = 0; Counter < NeighbourList.Num(); Counter++)
		{
			TempVector.X = X + NeighbourList[Counter].X;
			TempVector.Y = Y + NeighbourList[Counter].Y;

			///Checking boundary conditions
			if (CheckBoundary(TempVector))
			{

				if (!TempPathNode) delete TempPathNode;

				TempPathNode = new PathNode(TempVector,
					ClosedList[ClosedList.Num() - 1],
					StartIndex,
					TargetIndex);

				///Check 1: if the TempPathNode is the TargetIndex
				if (TempPathNode->CurrentPathNode.X != TargetIndex.X 
					&& TempPathNode->CurrentPathNode.Y != TargetIndex.Y
					&& AGeodroidGameMode::IsMapNodeWalkable(X,Y))
				{
					///Check 2: if the TempPathNode is not available in the ClosedList
					if (!ClosedList.Contains(TempPathNode))
					{
						///Find the index of TempPathNode in OpenedList
						int32 IndexOfExistingNode = OpenedList.Find(TempPathNode);

						///If the node does not exist
						if (IndexOfExistingNode == INDEX_NONE)
						{
							///Add Node to OpenedList
							OpenedList.Add(TempPathNode);
						}
						///Otherwise Check if earlier FCost is Higher
						else if (OpenedList[IndexOfExistingNode]->FCost > TempPathNode->FCost)
						{
							///Add the new Node to the OpenedList
							if(OpenedList[IndexOfExistingNode]) delete OpenedList[IndexOfExistingNode];
							OpenedList[IndexOfExistingNode] = TempPathNode;
						}
					}
					
				}
				else if (TempVector.X == TargetIndex.X && TempVector.Y == TargetIndex.Y)
				{
					MakePathList(TempPathNode);
					if (!TempPathNode) delete TempPathNode;
					return OutputList;
				}
			}
		}
		LoopCounter++;
	}

	UE_LOG(LogTemp, Warning, TEXT("Looped %d times"), LoopCounter);
	return TArray<FVector2D>();
}

bool UA_Pathfinding::CheckBoundary(FVector2D &TempVector) const
{
	return TempVector.X >= 0 &&
		TempVector.X < MapMaxSize.X &&
		TempVector.Y >= 0 &&
		TempVector.Y < MapMaxSize.Y;
}

void UA_Pathfinding::MakePathList(PathNode* FinalPathNode)
{
	OutputList.Empty();
	//Loop Till all the PathNodes in the PathNode LinkedList is transfered to OutputList
	while (FinalPathNode)
	{
		OutputList.Add(FinalPathNode->CurrentPathNode);
		FinalPathNode = FinalPathNode->PreviousPathNode;
	}
}



#pragma endregion
