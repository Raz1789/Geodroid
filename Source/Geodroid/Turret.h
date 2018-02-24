// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefenseStructures.h"
#include "WorldCollision.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GeodroidProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "PointerProtection.h"
#include "DrawDebugHelpers.h"
#include "MapClass.h"
#include "Turret.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	//Projectile class
	TSubclassOf<class AGeodroidProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		class USoundBase* FireSound;

	//Stores the Target Actor
	AActor* TargetActor = nullptr;

	//Time from last shot was fired
	float TimeFromLastFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Turret Rotator
		UStaticMeshComponent* BP_Turret;

	///***************** MEMBER FUNCTION ************************///
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "CPP Function", meta =  (DisplayName = "LookAtTargetEnemy"))
		//Rotate Turret to Look at Target Enemy
		void ReceiveLookAtTargetEnemy(const AActor* TargetEnemy);

	UFUNCTION()
	//Check if the Enemy inside the InfluenceCircle is visible and take necessary action
	void CheckAndExecuteAttack();

	UFUNCTION()
		//Shoot at Enemy
		void ShootAtEnemy(const  AActor* TargetEnemy);


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	//Inner Collision Box representing the Turret Area Restriction
	UBoxComponent* RestrictionArea;
	
	///**************** MEMBER FUNCTION ******************************///
	//Tick function
	virtual void Tick(float DeltaTime) override;
};
