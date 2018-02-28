// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefenseStructures.h"
#include "BaseEnemyClass.h"

#include "Components/BoxComponent.h"
#include "SlowTrap.generated.h"

/**
 * 
 */
UCLASS()
class GEODROID_API ASlowTrap : public ADefenseStructures
{
	GENERATED_BODY()

protected:
	///***************** MEMBER VARIABLE *******************///

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	//Overlap box
	UBoxComponent* InfluenceBox;

	//The World Component
	UWorld* World;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
public:
	ASlowTrap();
	
};
