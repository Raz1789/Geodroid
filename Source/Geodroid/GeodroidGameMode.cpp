// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GeodroidGameMode.h"
#include "GeodroidHUD.h"
#include "GeodroidCharacter.h"
#include "UObject/ConstructorHelpers.h"

/// STATIC MEMBER VARIABLES
FVector2D AGeodroidGameMode::MapMaxSize = FVector2D(9, 7);

TArray<bool> AGeodroidGameMode::MapWalkableArray;

TArray<FMapNode> AGeodroidGameMode::Map;

bool AGeodroidGameMode::bMapNodeStatusChanged;


/// MEMBER FUNCTIONS
AGeodroidGameMode::AGeodroidGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGeodroidHUD::StaticClass();

	///FPS TEMPLATE MODIFIED FROM HERE ON BELOW FOR THIS FUNCTION
	PrimaryActorTick.bCanEverTick = true;

	///Default Initializing for MapWalkableArray
	if (MapDesignWalkableArray.Num() == 0)
	{
		MapDesignWalkableArray.Add(FVector2D(0, -1));
		MapDesignWalkableArray.Add(FVector2D(1, -4));
		MapDesignWalkableArray.Add(FVector2D(2, -4));
		MapDesignWalkableArray.Add(FVector2D(2, 2));
		MapDesignWalkableArray.Add(FVector2D(3, -4));
		MapDesignWalkableArray.Add(FVector2D(3, 2));
		MapDesignWalkableArray.Add(FVector2D(4, 4));
		MapDesignWalkableArray.Add(FVector2D(4, 6));
		MapDesignWalkableArray.Add(FVector2D(5, 2));
		MapDesignWalkableArray.Add(FVector2D(5, 4));
		MapDesignWalkableArray.Add(FVector2D(6, 2));
		MapDesignWalkableArray.Add(FVector2D(6, 4));
	}

	TimeBetweenSpawn = 1.f;
	TimeFromLastSpawn = 0.f;
	PawnCounter = 0;
	MaxSpawnablePawns = 5;

	StartNode = FVector2D(0.f, 0.f);
	TargetNode = FVector2D(4.f, 5.f);
}

///FPS TEMPLATE CODE MODIFIED FROM HERE ON BELOW

void AGeodroidGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	World = GetWorld();

	if (IsDebugOn())
	{
		if (!World) UE_LOG(LogTemp, Error, TEXT("World Not Initialized"));
	}

	///Creating the Game map grid
	CreateMapGrid();
	
}

void AGeodroidGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PawnCounter < MaxSpawnablePawns)
	{
		TimeFromLastSpawn += DeltaTime;
	}

	//Check if the SpawnDelay is crossed && if PawnCounter is less than the MAX_SPAWNED_PAWN
	if ((TimeFromLastSpawn > TimeBetweenSpawn) && PawnCounter < MaxSpawnablePawns)
	{
		//Reset CurrentTimer
		TimeFromLastSpawn = 0.f;
		//Spawn an Enemy
		SpawnEnemy(0);
		//Increment PawnCounter
		PawnCounter++;
	}
}

int32 AGeodroidGameMode::IndexFrom1DTo2D(int32 X, int32 Y, FVector2D ArraySize)
{
	return ((X * ArraySize.Y) + Y);
}

FMapNode AGeodroidGameMode::WorldToMapNode(FVector WorldPosition)
{
	int32 X = std::max(MapMaxSize.X, WorldPosition.X);
	int32 Y = std::max(MapMaxSize.Y, WorldPosition.Y);
	return Map[IndexFrom1DTo2D(X,Y,MapMaxSize)];
}

void AGeodroidGameMode::CreateMapGrid()
{
	bMapNodeStatusChanged = true;
	///Creating a TArray of FMapNodes
	for (int32 x = 0; x < MapMaxSize.X; x++)
	{
		for (int32 y = 0; y < MapMaxSize.Y; y++)
		{
			FMapNode Temp;
			Temp.StructInit(FVector2D((float)x, (float)y), FVector(x * 400.f, y * 400.f, 64), true); //Generates a node on every floor panel (Since all floor panels are 400x400)
			Map.Add(Temp);
		}
	}

	///Setting the Walkables as per the game designer
	SetMapWalkables();

	///Extract the walkable array
	MapWalkableExtractor();

	if (IsDebugOn())
	{
		DisplayMapForDebug();
	}
}

void AGeodroidGameMode::DisplayMapForDebug()
{
	for (int32 x = 0; x < MapMaxSize.X; x++)
	{
		for (int32 y = 0; y < MapMaxSize.Y; y++)
		{
			UE_LOG(LogTemp, Warning, TEXT("(%d,%d): %s & %s"), x, y, *Map[IndexFrom1DTo2D(x, y, MapMaxSize)].Position.ToString(), *FString(Map[IndexFrom1DTo2D(x, y, MapMaxSize)].bWalkable ? "Walkable" : "Not Walkable"));
		}
	}
}

void AGeodroidGameMode::SetMapWalkables()
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

FVector AGeodroidGameMode::GetMapNodePosition(int32 X, int32 Y)
{
	return Map[IndexFrom1DTo2D(X, Y, MapMaxSize)].Position;
}

bool AGeodroidGameMode::IsMapNodeWalkable(int32 X, int32 Y)
{
	return Map[IndexFrom1DTo2D(X, Y, MapMaxSize)].bWalkable;
}

void AGeodroidGameMode::SetMapNodeWalkable(int32 X, int32 Y, bool _bWalkable)
{
	bMapNodeStatusChanged = true;
	Map[IndexFrom1DTo2D(X, Y, MapMaxSize)].bWalkable = _bWalkable;
	MapWalkableExtractor();
}

bool AGeodroidGameMode::IsDebugOn()
{
		return DEBUG;
}

bool AGeodroidGameMode::IsMapNodeStatusChanged()
{
	return bMapNodeStatusChanged;
}

TArray<bool> AGeodroidGameMode::GetMapWalkableArray()
{
	return MapWalkableArray;
}

FVector2D AGeodroidGameMode::GetMapMaxSize()
{
	return MapMaxSize;
}

void AGeodroidGameMode::MapWalkableExtractor()
{
	MapWalkableArray.Empty();
	for (int32 Counter = 0; Counter < Map.Num(); Counter++)
	{
		MapWalkableArray.Add(Map[Counter].bWalkable);
	}
}

void AGeodroidGameMode::SpawnEnemy(int32 PawnClassIndex)
{
	if (EnemyClassList.Num() != 0 && EnemyClassList[PawnClassIndex]) //EnemyClassList second since it prevent from crashing
	{
		if (World)
		{
			FVector position = FVector(StartNode.X, StartNode.Y, 60.f);

			//Spawn the actor at that location
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			EnemyList.Add(World->SpawnActor<ABaseEnemyClass>(EnemyClassList[PawnClassIndex], position, FRotator(0.f), SpawnParams));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GetWorld not initialized"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyClass Class not initialized"));

	}
}
