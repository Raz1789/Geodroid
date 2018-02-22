// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "PointerProtection.h"
#include "MapNode.h"
#include "A_Pathfinding.h"
#include "GeodroidProjectile.h"
#include "MapClass.h"
#include "GeodroidCharacter.h"
#include "BaseEnemyClass.generated.h"

UCLASS()
class GEODROID_API ABaseEnemyClass : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseEnemyClass();

protected:
	///MEMBER VARIABLE
	//Current Health of the Enemy
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

	
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Game Design")
	//Collision capsule
	//TODO work on the collision aspect
	USphereComponent* CollisionComponent = nullptr;

	///MEMBER FUNCTION
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Move to next Node Function
	void MovePawnAlongPathList(float DeltaTime);

	//Function to sent Gold to Player
	void SendResourceToPlayer();

	//Initial the death sequence
	void DeathSequence(float DeltaTime);

public:	
	///MEMBER VARIABLE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	//Speed of the Enemy
	float EnemyVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	//Max Health of the Enemy
	float MaxEnemyHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	//The amount of gold this enemy gives
	int32 EnemyGold;


	struct TimerContainer
	{
		float MaxTime;
		float CurrentTime;
	};

	//Custom Time Container for setting the DeathtimeCounter
	TimerContainer DeathTimer;

	///MEMBER FUNCTION

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	//Subtracts damage from health
	void ApplyDamage(float Damage);
	
	UFUNCTION()
	//Function to Update the PathList
	void UpdatePathList();

	UFUNCTION(BlueprintCallable)
	//Get Current Health from 0 - 1 float
	float GetHealth();

	UFUNCTION()
	//Function to activate if the Enemy is hit
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
};
