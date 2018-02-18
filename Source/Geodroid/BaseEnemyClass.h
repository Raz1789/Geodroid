// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MapNode.h"
#include "A_Pathfinding.h"
#include "MapClass.h"
#include "BaseEnemyClass.generated.h"

UCLASS()
class GEODROID_API ABaseEnemyClass : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseEnemyClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//Get a Pathfinding Object for this Enemy
	UA_Pathfinding* Pathfinder;

	//Contain the path that the pawn should follow
	TArray<FVector2D> PathList;

	//A counter storing the index of the target node from the PathList
	int32 PathCounter;

	//Variable storing the previous Node in the Tick Iteration
	FMapNode PreviousNode;

	//Variable storing the current Node of the Pawn
	FMapNode CurrentNode;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Move to next Node Function

	
};
