// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "MapNode.h"
#include "A_Pathfinding.h"
#include "BaseEnemyController.h"
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
	///MEMBER VARIABLE
	//Health of the Enemy
	float EnemyHealth;

	bool IsEnemyDead;

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

	///MEMBER FUNCTION
	//Move to next Node Function
	void MovePawnAlongPathList(float DeltaTime);

public:	
	///MEMBER VARIABLE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	//Speed of the Enemy
	float EnemyVelocity;

	UFUNCTION()
	//Subtracts damage from health
	void ApplyDamage(float Damage);

	///MEMBER FUNCTION

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//Function to Update the PathList
	void UpdatePathList();
	

	
};
