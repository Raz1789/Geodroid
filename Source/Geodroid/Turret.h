// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefenseStructures.h"
#include "WorldCollision.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "PointerProtection.h"
#include "DrawDebugHelpers.h"
#include "MapClass.h"
#include "Turret.generated.h"

/**
 * 
 */

UCLASS()
class GEODROID_API ATurret : public ADefenseStructures
{
	GENERATED_BODY()

protected:
	///***************** MEMBER VARIABLE *******************///
	//CollisionShape for Sweeping for enemies
	FCollisionShape InfluenceCircle;

	//Array of Enemies inside the InfluenceCircle
	TArray<FHitResult> OutHits;

	//The World Component
	UWorld* World;

	//The Actor Location
	FVector ActorLocation;

	//Actor Node in Map
	FVector2D CurrentMapNode;

	//Stores the Target Actor
	AActor* TargetActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Getter Variable")
	//Turret Rotator
	FRotator TurretRotator;

	///***************** MEMBER FUNCTION ************************///
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "CPP Function", meta =  (DisplayName = "LookAtTargetEnemy"))
		//Rotate Turret to Look at Target Enemy
		void ReceiveLookAtTargetEnemy(const AActor* TargetEnemy);

	UFUNCTION()
	//Check if the Enemy inside the InfluenceCircle is visible and take necessary action
	void CheckAndExecuteAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


public:
	///*************** CONSTRUCTOR *************************************///
	ATurret();

	///***************** MEMBER VARIABLE *****************************///
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Design")
	//Influence Circle radius value for Game Design Purposes
	float InfluenceCircleRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Design")
	//Color of Material
	FColor MaterialColor;

	//Inner Collision Box representing the Turret Area Restriction
	UBoxComponent* RestrictionArea;
	
	///**************** MEMBER FUNCTION ******************************///
	//Tick function
	virtual void Tick(float DeltaTime) override;
};
