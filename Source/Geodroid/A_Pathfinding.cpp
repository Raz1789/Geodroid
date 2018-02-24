// Fill out your copyright notice in the Description page of Project Settings.

#include "A_Pathfinding.h"

#define OUT

#pragma region STRUCT FUNCTION DEFINITIONS

UA_Pathfinding::PathNode::PathNode(FVector2D _CurrentPathNode, PathNode* _PreviousPathNode, FVector2D _StartPathNode, FVector2D _TargetPathNode)
{
	CurrentMapNode = _CurrentPathNode;
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
	if (this != &other) {
		if (CurrentMapNode.X == other.CurrentMapNode.X && CurrentMapNode.Y == other.CurrentMapNode.Y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else 
	{
		return true;
	}
	
}

void UA_Pathfinding::PathNode::CalculateCost(FVector2D StartPathNode, FVector2D TargetPathNode)
{
	GCost = FVector2D::DistSquared(StartPathNode, CurrentMapNode);
	HCost = FVector2D::DistSquared(TargetPathNode, CurrentMapNode);
	FCost = GCost + HCost;
}

UA_Pathfinding::PathNode::~PathNode()
{
	delete PreviousPathNode;
}

#pragma endregion

#pragma region CLASS MEMBER FUNCTION DEFINITIONS

TArray<FVector2D> UA_Pathfinding::CalculatePath(FVector2D StartIndex,
	FVector2D TargetIndex, bool bPathExistCheck)
{
	///Initializing NeighbourList only first time
	if (NeighbourList.Num() == 0)
	{
		NeighbourList.Add(FVector2D(-1, 0));
		NeighbourList.Add(FVector2D(0, -1));
		NeighbourList.Add(FVector2D(1, 0));
		NeighbourList.Add(FVector2D(0, 1));
	}
	
	///If MapNodeStatus has Changed or if this is the initial run of this function
	if (UMapClass::IsMapNodeStatusChanged() || MapWalkableArray.Num() == 0)
	{
		MapWalkableArray.Reset(0);
		///Make a copy of the MapWalkableArray in this class and get the MapSize
		MapWalkableArray.Append(UMapClass::GetMapWalkableArray());
		MapMaxSize = UMapClass::GetMapMaxSize();
	}

	///Creating the temporary variables
	TArray<PathNode*> OpenedList;
	TArray<PathNode*> ClosedList;

	///Create a temporary PathNode for initial location
	PathNode* TempPathNode = new PathNode(StartIndex, nullptr, StartIndex, TargetIndex);

	///Temporary FVector2D for Speed Optimization
	FVector2D CheckingNodeVector;

	///Assign it to OpenedList
	OpenedList.Add(TempPathNode);
	int32 LoopCounter = 0;


	///Loop Till OpenedList is not Empty
	while (OpenedList.Num() > 0 && LoopCounter < 50)
	{
		///Check if the TempNode is the TargetNode
		if (TempPathNode->CurrentMapNode.X == TargetIndex.X && TempPathNode->CurrentMapNode.Y == TargetIndex.Y)
		{
			///Make the OutputList from TempPathnode LinkedList
			return MakePathList(TempPathNode);
		}
		else
		{
			///Sort OpenedList, assign smallest to ClosedList and remove from OpenList
			OpenedList.Sort();
			ClosedList.Add(OpenedList[0]);
			OpenedList.RemoveAt(0);

			///Getting X and Y of the node that needs to be checked
			int32 X = ClosedList[ClosedList.Num() - 1]->CurrentMapNode.X;
			int32 Y = ClosedList[ClosedList.Num() - 1]->CurrentMapNode.Y;

			///Searching the Valid Neighbours for Least FCost
			for (int32 Counter = 0; Counter < NeighbourList.Num(); Counter++)
			{
				CheckingNodeVector.X = X + NeighbourList[Counter].X;
				CheckingNodeVector.Y = Y + NeighbourList[Counter].Y;

				///Checking boundary conditions
				if (CheckBoundary(CheckingNodeVector))
				{
					TempPathNode = new PathNode(CheckingNodeVector,
												ClosedList[ClosedList.Num() - 1],
												StartIndex,
												TargetIndex);

					///Check 1: if the TempPathNode is the TargetIndex
					if ((TempPathNode->CurrentMapNode.X != TargetIndex.X
						|| TempPathNode->CurrentMapNode.Y != TargetIndex.Y)
						&& UMapClass::IsMapNodeWalkable(CheckingNodeVector.X, CheckingNodeVector.Y))
					{
						///Check 2: if the TempPathNode is not available in the ClosedList
						if (!ClosedList.ContainsByPredicate(
							///LAMBDA FUNCTION to check equality
							[TempPathNode](const PathNode* CheckNode)
								{
									return ((TempPathNode->CurrentMapNode.X == CheckNode->CurrentMapNode.X)
											&& (TempPathNode->CurrentMapNode.Y == CheckNode->CurrentMapNode.Y));
								}
							))
						{
							///Find the index of TempPathNode in OpenedList
							int32 IndexOfExistingNode = -1;

							for (int32 OpenedCounter = 0; OpenedCounter < OpenedList.Num(); OpenedCounter++)
							{
								if (*OpenedList[OpenedCounter] == *TempPathNode)
								{
									IndexOfExistingNode = OpenedCounter;
									break;
								}
							}
							///Check 3: if the TempPathNode is not available in the OpenedList
							if (IndexOfExistingNode == -1)
							{
								///Add Node to OpenedList
								OpenedList.Add(TempPathNode);
							}
							///Otherwise Check if earlier FCost is Higher
							else if (OpenedList[IndexOfExistingNode]->FCost > TempPathNode->FCost)
							{
								///Add the new Node to the OpenedList
								if (OpenedList[IndexOfExistingNode]) delete OpenedList[IndexOfExistingNode];
								OpenedList[IndexOfExistingNode] = TempPathNode;
							}
						}

					}
					///If target reached
					else if (CheckingNodeVector.X == TargetIndex.X && CheckingNodeVector.Y == TargetIndex.Y)
					{
						///Return the OutputList
						return MakePathList(TempPathNode);
					}
				}
			}
			LoopCounter++;
		}
	}
	return TArray<FVector2D>();
}

bool UA_Pathfinding::CheckBoundary(FVector2D &TempVector) const
{
	return TempVector.X >= 0 &&
		TempVector.X < MapMaxSize.X &&
		TempVector.Y >= 0 &&
		TempVector.Y < MapMaxSize.Y;
}

TArray<FVector2D> UA_Pathfinding::MakePathList(PathNode* FinalPathNode)
{
	TArray<FVector2D> OutputList;

	//Loop Till all the PathNodes in the PathNode LinkedList is transfered to OutputList
	while (FinalPathNode)
	{
		OutputList.Add(FinalPathNode->CurrentMapNode);
		FinalPathNode = FinalPathNode->PreviousPathNode;
	}

	return OutputList;
}

bool UA_Pathfinding::PathExist(FVector2D StartIndex)
{
	return true;
}

#pragma endregion
