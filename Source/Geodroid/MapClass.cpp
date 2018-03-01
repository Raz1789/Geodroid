// Fill out your copyright notice in the Description page of Project Settings.

#include "MapClass.h"

#define DEBUG false;

///***********************************************************************************************************///
///                                   STATIC VARIABLE DECLARATION
///***********************************************************************************************************////

///-------------------------------------- MAP DESIGN VARIABLES ---------------------------------------------------///
FVector2D UMapClass::MapMaxSize = FVector2D(9, 7);

float UMapClass::WorldNodeSize = 400.f;

TArray<FVector2D> UMapClass::MapDesignWalkableArray;

///-------------------------------------- MAP INFORMATION VARIABLES ---------------------------------------------------///
TArray<FMapNode> UMapClass::Map;

TArray<bool> UMapClass::MapWalkableArray;

TArray<bool> UMapClass::MapStructurePresentArray;

bool UMapClass::bMapNodeStatusChanged;

FMapNode UMapClass::TargetNode;



///***********************************************************************************************************///
///                                           PRIVATE CONSTRUCTOR
///***********************************************************************************************************////
UMapClass::UMapClass() {} //So that no one can make an instance of this static class

///***********************************************************************************************************///
///											 CLASS INITIALIZOR FUNCTION
///***********************************************************************************************************////
void UMapClass::Init(TArray<FVector2D> _MapDesignWalkableArray, float _WorldNodeSize, FVector2D _TargetNode, FVector2D _MapMaxSize)
{
	///Array overflow protector
	MapDesignWalkableArray.Empty();
	MapDesignWalkableArray.Append(_MapDesignWalkableArray);

	WorldNodeSize = _WorldNodeSize;

	MapMaxSize = _MapMaxSize;
	
	///Creating the Game map grid
	CreateMapGrid();

	///Create the Target Node after the Map is created
	TargetNode = Map[IndexFrom2DTo1D(_TargetNode.X, _TargetNode.Y, MapMaxSize)];
}

///***********************************************************************************************************///
///											 MAP CREATOR FUNCTION
///***********************************************************************************************************////
void UMapClass::CreateMapGrid()
{
	///Array overflow protector
	Map.Empty();

	bMapNodeStatusChanged = true;

	///Creating a TArray of FMapNodes
	for (int32 x = 0; x < MapMaxSize.X; x++)
	{
		for (int32 y = 0; y < MapMaxSize.Y; y++)
		{
			FMapNode TempNode;

			///Generates a temp node on every floor panel with default walkable to true
			TempNode.StructInit(FVector2D((float)x, (float)y),
							FVector(x * WorldNodeSize, y * WorldNodeSize, 64),
							true); 
			///Add to the Map
			Map.Add(TempNode);
		}
	}

	///Setting the Walkables as per the game designer
	SetMapWalkables();

	///Extract the walkable variable from Map to MapWalkableArray
	MapWalkableExtractor();

	///Initialize if a structure is present on the node as false at start
	MapStructurePresentArray.Init(false, Map.Num());

	///Displays the Map Node Details in the debug log
	if (IsDebugOn())
	{
		DisplayMapForDebug();
	}
}

///***********************************************************************************************************///
///					SET TEH MAP WALKABLE DETAILS AS PER DESIGNER INPUTS
///***********************************************************************************************************////
void UMapClass::SetMapWalkables()
{
	///Iterate through the MapWalkableArray
	for (int32 Counter = 0; Counter < MapDesignWalkableArray.Num(); Counter++)
	{
		///If X index is valid
		if (MapDesignWalkableArray[Counter].X < MapMaxSize.X)
		{
			///If the Y index is negative & Valid
			if (MapDesignWalkableArray[Counter].Y < 0 
				&& (MapDesignWalkableArray[Counter].Y * -1) < MapMaxSize.Y)
			{
				///Then Iterates through the rest of the Map index in Y and fill bWalkable with false
				for (int32 Y = (MapDesignWalkableArray[Counter].Y * -1); Y < MapMaxSize.Y; Y++)
				{
					///Set the bWalkable to false
					Map[IndexFrom2DTo1D(MapDesignWalkableArray[Counter].X,
										Y,
										MapMaxSize)].bWalkable = false;
				}

			}
			///If Y index is Positive & Valid
			else if (MapDesignWalkableArray[Counter].Y >= 0 
					 && (MapDesignWalkableArray[Counter].Y * -1) < MapMaxSize.Y)
			{
				///Set the bWalkable to false
				Map[IndexFrom2DTo1D(MapDesignWalkableArray[Counter].X,
									MapDesignWalkableArray[Counter].Y,
									MapMaxSize)].bWalkable = false;

			}
			///Else Y index is invalid
			else
			{
				UE_LOG(LogTemp, Error, TEXT("(%s) Index for Map is not Correct @ MapWalkableArray Index %d"),
					   *MapDesignWalkableArray[Counter].ToString(), Counter);
			}
		}
		///Else X index is invalid
		else
		{
			UE_LOG(LogTemp, Error, TEXT("(%s) Index for Map is not Correct @ MapWalkableArray Index %d"),
				   *MapDesignWalkableArray[Counter].ToString(), Counter);
		}
	}

}

///***********************************************************************************************************///
///	  EXTRACTS TEH MAP WALKABLE DETAILS AND STORES IN A SEPARATE ARRAY FOR EASY ACCESS
///***********************************************************************************************************////
void UMapClass::MapWalkableExtractor()
{
	///Array Overflow protection
	MapWalkableArray.Empty();

	///Extraction
	for (int32 Counter = 0; Counter < Map.Num(); Counter++)
	{
		MapWalkableArray.Add(Map[Counter].bWalkable);
	}
}


///***********************************************************************************************************///
///										GETTERS
///***********************************************************************************************************////

///----------------------- RETURNS A FVECTOR WITH THE POSITION OF THE NODE IN THE WORLD ----------------------///
FVector UMapClass::GetMapNodePosition(int32 X, int32 Y)
{
	return Map[IndexFrom2DTo1D(X, Y, MapMaxSize)].Position;
}

///----------------------- RETURNS A MAPNODE PERTAINING TO THE WORLD POSITION PROVDIDED ----------------------///
FMapNode UMapClass::WorldToMapNode(FVector WorldPosition)
{
	int32 X = std::min(MapMaxSize.X - 1, WorldPosition.X / WorldNodeSize);
	int32 Y = std::min(MapMaxSize.Y - 1, WorldPosition.Y / WorldNodeSize);
	return Map[IndexFrom2DTo1D(X, Y, MapMaxSize)];
}

///----------------------- RETURNS TRUE IF THE NODE IS WALKABLE ----------------------------------------------///
bool UMapClass::IsMapNodeWalkable(int32 X, int32 Y)
{
	return Map[IndexFrom2DTo1D(X, Y, MapMaxSize)].bWalkable;
}

///----------------------- RETURNS TRUE IF THERE IS A STRUCTURE PLACED ON THIS NODE ---------------------------///
bool UMapClass::IsStructureOnNode(int32 X, int32 Y)
{
	return MapStructurePresentArray[IndexFrom2DTo1D(X, Y, MapMaxSize)];
}

///----- RETURNS TRUE IF DURING THE EXECUTION OF THE GAME, THE NODE WALKABLE STATUS HAS CHANGED --------------///
bool UMapClass::IsMapNodeStatusChanged()
{
	return bMapNodeStatusChanged;
}

///---------------------- GET THE WALKABLE ARRAY -------------------------------------------------------------///
TArray<bool> UMapClass::GetMapWalkableArray()
{
	return MapWalkableArray;
}

///---------------------- GET THE MAP SIZE IN NUMBER OF NODES -------------------------------------------------///
FVector2D UMapClass::GetMapMaxSize()
{
	return MapMaxSize;
}

///---------------------- GET TARGET NODE FOR THE MAP ---------------------------------------------------------///
FMapNode UMapClass::GetTargetNode()
{
	return TargetNode;
}

///---------------------- GET SIZE OF THE NODE IN WORLD DIMENSIONS --------------------------------------------///
float UMapClass::GetWorldNodeSize()
{
	return WorldNodeSize;
}

///---------------------- RETURNS TRUE IF THE DEBUG MODE IS TRUE ----------------------------------------------///
bool UMapClass::IsDebugOn()
{
	return DEBUG;
}


///***********************************************************************************************************///
///										SETTERS
///***********************************************************************************************************////

///---------------------- UPDATE THE STRUCTURE BUILD STATUS OF THE NODE ---------------------------------------///
void UMapClass::SetStructureOnNode(int32 X, int32 Y, bool _bStructurePresent)
{
	MapStructurePresentArray[IndexFrom2DTo1D(X, Y, MapMaxSize)] = _bStructurePresent;
}

///---------------------- UPDATE THE WALKABLE STATUS OF THE NODE ----------------------------------------------///
void UMapClass::SetMapNodeWalkable(int32 X, int32 Y, bool _bWalkable)
{
	bMapNodeStatusChanged = true;
	Map[IndexFrom2DTo1D(X, Y, MapMaxSize)].bWalkable = _bWalkable;
	MapWalkableArray[IndexFrom2DTo1D(X, Y, MapMaxSize)] = _bWalkable;
}

///---------------------- RESET THE NODE CHANGE STATUS -------------------------------------------------------///
void UMapClass::ResetMapNodeChangeStatus()
{
	bMapNodeStatusChanged = false;
}

///***********************************************************************************************************///
///										HELPER FUNCTIONS
///***********************************************************************************************************////

///---------------------- CONVERT A 2D INDEX TO 1D -----------------------------------------------------------///
int32 UMapClass::IndexFrom2DTo1D(int32 X, int32 Y, FVector2D ArraySize)
{
	return ((X * ArraySize.Y) + Y);
}

///---------------------- DISPLAYS THE MAP IN OUTPUT LOG FOR DEBUG --------------------------------------------///
void UMapClass::DisplayMapForDebug()
{
	for (int32 x = 0; x < MapMaxSize.X; x++)
	{
		for (int32 y = 0; y < MapMaxSize.Y; y++)
		{
			UE_LOG(LogTemp, Warning, TEXT("(%d,%d): %s & %s"), x, y,
				   *Map[IndexFrom2DTo1D(x, y, MapMaxSize)].Position.ToString(),
				   *FString(Map[IndexFrom2DTo1D(x, y, MapMaxSize)].bWalkable ? "Walkable" : "Not Walkable"));
		}
	}
}