// Fill out your copyright notice in the Description page of Project Settings.

#include "MapClass.h"

#define DEBUG true;

/// STATIC MEMBER VARIABLES
FVector2D UMapClass::MapMaxSize = FVector2D(9, 7);

TArray<bool> UMapClass::MapWalkableArray;

TArray<bool> UMapClass::MapStructurePresentArray;

TArray<FMapNode> UMapClass::Map;

bool UMapClass::bMapNodeStatusChanged;

TArray<FVector2D> UMapClass::MapDesignWalkableArray;

float UMapClass::WorldNodeSize = 400.f;

FMapNode UMapClass::TargetNode;

///MEMBER FUNCTIONS

void UMapClass::Init(TArray<FVector2D> _MapDesignWalkableArray, float _WorldNodeSize, FVector2D _TargetNode, FVector2D _MapMaxSize)
{
	MapDesignWalkableArray.Empty();
	MapDesignWalkableArray.Append(_MapDesignWalkableArray);

	WorldNodeSize = _WorldNodeSize;

	MapMaxSize = _MapMaxSize;
	
	///Creating the Game map grid
	CreateMapGrid();

	///Create the Target Node after the Map is created
	TargetNode = Map[IndexFrom1DTo2D(_TargetNode.X, _TargetNode.Y, MapMaxSize)];
}

int32 UMapClass::IndexFrom1DTo2D(int32 X, int32 Y, FVector2D ArraySize)
{
	return ((X * ArraySize.Y) + Y);
}

FMapNode UMapClass::WorldToMapNode(FVector WorldPosition)
{
	int32 X = std::min(MapMaxSize.X - 1, WorldPosition.X / WorldNodeSize);
	int32 Y = std::min(MapMaxSize.Y - 1, WorldPosition.Y / WorldNodeSize);
	return Map[IndexFrom1DTo2D(X,Y, MapMaxSize)];
}

void UMapClass::CreateMapGrid()
{
	bMapNodeStatusChanged = true;
	///Creating a TArray of FMapNodes
	for (int32 x = 0; x < MapMaxSize.X; x++)
	{
		for (int32 y = 0; y < MapMaxSize.Y; y++)
		{
			FMapNode Temp;
			Temp.StructInit(FVector2D((float)x, (float)y), FVector(x * WorldNodeSize, y * WorldNodeSize, 64), true); //Generates a node on every floor panel
			Map.Add(Temp);
		}
	}

	///Setting the Walkables as per the game designer
	SetMapWalkables();

	///Extract the walkable array
	MapWalkableExtractor();

	MapStructurePresentArray.Init(false, Map.Num());

	if (IsDebugOn())
	{
	//	DisplayMapForDebug();
	}
}

void UMapClass::DisplayMapForDebug()
{
	for (int32 x = 0; x < MapMaxSize.X; x++)
	{
		for (int32 y = 0; y < MapMaxSize.Y; y++)
		{
			UE_LOG(LogTemp, Warning, TEXT("(%d,%d): %s & %s"), x, y, 
				   *Map[IndexFrom1DTo2D(x, y, MapMaxSize)].Position.ToString(),
				   *FString(Map[IndexFrom1DTo2D(x, y, MapMaxSize)].bWalkable ? "Walkable" : "Not Walkable"));
		}
	}
}

void UMapClass::SetMapWalkables()
{
	///Iterate through the MapWalkableArray
	for (int32 Counter = 0; Counter < MapDesignWalkableArray.Num(); Counter++)
	{
		///If X index is valid
		if (MapDesignWalkableArray[Counter].X < MapMaxSize.X)
		{
			///If the Y index is negative & Valid
			if (MapDesignWalkableArray[Counter].Y < 0 && (MapDesignWalkableArray[Counter].Y * -1) < MapMaxSize.Y)
			{
				///Then Iterates through the rest of the Map index in Y and fill bWalkable with false
				for (int32 Y = (MapDesignWalkableArray[Counter].Y * -1); Y < MapMaxSize.Y; Y++)
				{
					///Set the bWalkable to false
					Map[IndexFrom1DTo2D(MapDesignWalkableArray[Counter].X, Y, MapMaxSize)].bWalkable = false;
				}

			}
			///If Y index is Positive & Valid
			else if (MapDesignWalkableArray[Counter].Y >= 0 && (MapDesignWalkableArray[Counter].Y * -1) < MapMaxSize.Y)
			{
				///Set the bWalkable to false
				Map[IndexFrom1DTo2D(MapDesignWalkableArray[Counter].X, MapDesignWalkableArray[Counter].Y, MapMaxSize)].bWalkable = false;

			}
			///Else Y index is invalid
			else
			{
				UE_LOG(LogTemp, Error, TEXT("(%s) Index for Map is not Correct @ MapWalkableArray Index %d"), *MapDesignWalkableArray[Counter].ToString(), Counter);
			}
		}
		///Else X index is invalid
		else
		{
			UE_LOG(LogTemp, Error, TEXT("(%s) Index for Map is not Correct @ MapWalkableArray Index %d"), *MapDesignWalkableArray[Counter].ToString(), Counter);
		}
	}

}

FVector UMapClass::GetMapNodePosition(int32 X, int32 Y)
{
	return Map[IndexFrom1DTo2D(X, Y, MapMaxSize)].Position;
}

bool UMapClass::IsMapNodeWalkable(int32 X, int32 Y)
{
	return Map[IndexFrom1DTo2D(X, Y, MapMaxSize)].bWalkable;
}

bool UMapClass::IsStructureOnNode(int32 X, int32 Y)
{
	return MapStructurePresentArray[IndexFrom1DTo2D(X, Y, MapMaxSize)];
}

void UMapClass::SetStructureOnNode(int32 X, int32 Y, bool _bStructurePresent)
{
	MapStructurePresentArray[IndexFrom1DTo2D(X, Y, MapMaxSize)] = _bStructurePresent;
}

void UMapClass::SetMapNodeWalkable(int32 X, int32 Y, bool _bWalkable)
{
	bMapNodeStatusChanged = true;
	Map[IndexFrom1DTo2D(X, Y, MapMaxSize)].bWalkable = _bWalkable;
	MapWalkableArray[IndexFrom1DTo2D(X, Y, MapMaxSize)] = _bWalkable;
}

bool UMapClass::IsDebugOn()
{
	return DEBUG;
}

bool UMapClass::IsMapNodeStatusChanged()
{
	return bMapNodeStatusChanged;
}

TArray<bool> UMapClass::GetMapWalkableArray()
{
	return MapWalkableArray;
}

FVector2D UMapClass::GetMapMaxSize()
{
	return MapMaxSize;
}

FMapNode UMapClass::GetTargetNode()
{
	return TargetNode;
}

float UMapClass::GetWorldNodeSize()
{
	return WorldNodeSize;
}

void UMapClass::ResetMapNodeChangeStatus()
{
	bMapNodeStatusChanged = false;
}

void UMapClass::MapWalkableExtractor()
{
	MapWalkableArray.Empty();
	for (int32 Counter = 0; Counter < Map.Num(); Counter++)
	{
		MapWalkableArray.Add(Map[Counter].bWalkable);
	}
}


