// Fill out your copyright notice in the Description page of Project Settings.

#include "A_Pathfinding.h"

#define OUT

/******************************* STRUCTURE DEFINITIONS **********************************************************/
#pragma region STRUCT FUNCTION DEFINITIONS

///***********************************************************************************************************///
///                                     STRUCTURE CONSTRUCTOR
///***********************************************************************************************************////
UA_Pathfinding::PathNode::PathNode(FVector2D _CurrentPathNode,
								   PathNode* _PreviousPathNode,
								   FVector2D _StartPathNode,
								   FVector2D _TargetPathNode)
{
	CurrentMapNode = _CurrentPathNode;
	PreviousPathNode = _PreviousPathNode;
	CalculateCost(_StartPathNode, _TargetPathNode);
}


///***********************************************************************************************************///
///                           OPERATOR "<" OVERLOADING FOR SORT FUNCTION IN TARRAY
///***********************************************************************************************************////
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

///***********************************************************************************************************///
///                           OPERATOR "==" OVERLOADING FOR CONTAINS FUNCTION IN TARRAY
///***********************************************************************************************************////
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


///***********************************************************************************************************///
///                          CALCULATES THE H, G AND F COST FOR THE NODE
///***********************************************************************************************************////
void UA_Pathfinding::PathNode::CalculateCost(FVector2D StartPathNode, FVector2D TargetPathNode)
{
	GCost = FVector2D::DistSquared(StartPathNode, CurrentMapNode);
	HCost = FVector2D::DistSquared(TargetPathNode, CurrentMapNode);
	FCost = GCost + HCost;
}


///***********************************************************************************************************///
///                                  DESTRUCTOR
///***********************************************************************************************************////
UA_Pathfinding::PathNode::~PathNode()
{
	delete PreviousPathNode; //delete the Previous path node upon deletion of this structures
}

#pragma endregion



/******************************* CLASS DEFINITIONS *************************************************************/
#pragma region CLASS MEMBER FUNCTION DEFINITIONS


///***********************************************************************************************************///
///                                  CONSTRUCTOR
///***********************************************************************************************************////
UA_Pathfinding::UA_Pathfinding()
{
	//Creating the Neighbours list
	NeighbourOffsetList.Add(FVector2D(-1, 0));
	NeighbourOffsetList.Add(FVector2D(0, -1));
	NeighbourOffsetList.Add(FVector2D(1, 0));
	NeighbourOffsetList.Add(FVector2D(0, 1));

	MapMaxSize = FVector2D(15.f, 7.f); //INITIALIZING WITH DEFAULT MAXSIZE
}

///***********************************************************************************************************///
///          CHECKS IF PATH IS NOT BLOCKED IF A STRUCTURE IS PLACED IN THE START INDEX
///***********************************************************************************************************////
bool UA_Pathfinding::CheckPathBlocked(FVector2D StartIndex)
{
	//Variable to discard
	TArray<FVector2D> OutPathList;

	bool bPathExist = false;

	//check individual neighbors of the start node to see if a path exist to the each neighbour
	for (FVector2D& NeighbourOffset : NeighbourOffsetList)
	{
		//Get the Neighbour Node
		FVector2D CheckingNode = StartIndex + NeighbourOffset;

		//Checking for boundary condition
		bool bIsNodeInsideMap = CheckBoundary(CheckingNode);

		if (bIsNodeInsideMap)
		{
			//Checking if the neighbour is walkable
			bool IsCheckNodeWalkable = UMapClass::IsMapNodeWalkable(CheckingNode.X, CheckingNode.Y);
			if (IsCheckNodeWalkable)
			{
				//Check if a path exist from this neighbour
				bool bIsPathAvailable = CalculatePathList(OutPathList, CheckingNode);
				if (!bIsPathAvailable)
				{
					return false;
				}
			}
		}
	}

	return true;
}

///***********************************************************************************************************///
///                              RETURNS THE PATH NODE INDEX LIST
///***********************************************************************************************************////
TArray<FVector2D> UA_Pathfinding::GetPathList(FVector2D StartIndex,	FVector2D EndIndex)
{
	//create an out parameter for the pathlist
	TArray<FVector2D> OutPathList;

	//check if a path exist
	bool bPathExist;
	bPathExist = CalculatePathList(OutPathList, StartIndex, EndIndex, false);

	if (bPathExist)
	{
		return OutPathList; //return pathlist if path exist
	}
	else
	{
		return TArray<FVector2D>(); //return empty array if path does not exist
	}
}


///***********************************************************************************************************///
///                              THE MAIN A* PATHFINDING ALGORITHM
///***********************************************************************************************************////
bool UA_Pathfinding::CalculatePathList(TArray<FVector2D>& OutPathList, const FVector2D& StartIndex, const FVector2D& EndIndex, bool bIsThisACheckOnly)
{
	///Making sure that the OutPathList is empty
	OutPathList.Empty();

	///**************** TEMP VARIABLES FOR CALCULATING A* PATHFINDING *************************/
	TArray<bool> MapWalkableArray;

	///Make a copy of the MapWalkableArray in this class and get the MapSize
	MapWalkableArray.Append(UMapClass::GetMapWalkableArray());
	MapMaxSize = UMapClass::GetMapMaxSize();

	///Creating the Open and Close List
	TArray<PathNode*> OpenedList;
	TArray<PathNode*> ClosedList;

	///Create a temporary PathNode for initial location
	//Temp PathNode for checking
	PathNode* TempPathNode = new PathNode(StartIndex, nullptr, StartIndex, EndIndex);

	///Temporary FVector2D for Speed Optimization
	FVector2D CheckingNodeVector;

	///Assign it to OpenedList
	OpenedList.Add(TempPathNode);
	int32 LoopCounter = 0;


	///Loop Till OpenedList is not Empty (CHECK TILL ALL POSSIBLE NODES CHECKED)
	while (OpenedList.Num() > 0 && LoopCounter < 50)
	{
		///Check if the TempNode is the TargetNode (To save unnecessary interations)
		if (TempPathNode->CurrentMapNode.X == EndIndex.X && TempPathNode->CurrentMapNode.Y == EndIndex.Y)
		{
			if (!bIsThisACheckOnly)
			{
				///Make the OutputList from TempPathnode LinkedList
				OutPathList.Append(MakePathList(TempPathNode));
			}
				return true;
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
			for (int32 Counter = 0; Counter < NeighbourOffsetList.Num(); Counter++)
			{
				CheckingNodeVector.X = X + NeighbourOffsetList[Counter].X;
				CheckingNodeVector.Y = Y + NeighbourOffsetList[Counter].Y;

				///Checking boundary conditions
				if (CheckBoundary(CheckingNodeVector))
				{
					TempPathNode = new PathNode(CheckingNodeVector,
												ClosedList[ClosedList.Num() - 1],
												StartIndex,
												EndIndex);

					///Check 1: if the TempPathNode is the EndIndex
					if ((TempPathNode->CurrentMapNode.X != EndIndex.X
						|| TempPathNode->CurrentMapNode.Y != EndIndex.Y)
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
					else if (CheckingNodeVector.X == EndIndex.X && CheckingNodeVector.Y == EndIndex.Y)
					{
						if (!bIsThisACheckOnly)
						{
							///Make the OutputList from TempPathnode LinkedList
							OutPathList.Append(MakePathList(TempPathNode));
						}
						return true;
					}
				}
			}
			LoopCounter++;
		}
	}

	return false;
}

///***********************************************************************************************************///
///                        FUNCTION TO CHECK THE MAP BOUNDARY CONDITIONS
///***********************************************************************************************************////
bool UA_Pathfinding::CheckBoundary(FVector2D &TempVector) const
{
	bool bIsNodeInsideMap = TempVector.X >= 0 &&
		TempVector.X < MapMaxSize.X &&
		TempVector.Y >= 0 &&
		TempVector.Y < MapMaxSize.Y;
	UE_LOG(LogTemp, Warning, TEXT(""));
	return bIsNodeInsideMap;
}

///***********************************************************************************************************///
///                       MAKES THE PATH FROM THE PASSED LINKED LIST
///***********************************************************************************************************////
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

#pragma endregion
