// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UNREAL HEADER FILES
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

// MANDATE FILES
#include "NodeViewerActor.generated.h"

/*****************************************************************************************************************
* CLASS NAME:	ANODEVIEWERACTOR
* DESCRIPTION:	Just a dummy Actor for visualizing the Nodes centers in the world
*****************************************************************************************************************/

UCLASS()
class GEODROID_API ANodeViewerActor : public AActor
{
	GENERATED_BODY()
	
protected:
	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER VARIABLE
	///***********************************************************************************************************///

	///-------------------------------------- MESH VARIABLE ------------------------------------------------------///
	UPROPERTY(EditAnywhere, Category = "Visualization Mesh")
		USphereComponent* SphereMesh;

public:	
	///***********************************************************************************************************///
	///                                       PUBLIC MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///-------------------------------------- CONSTRUCTOR --------------------------------------------------------///S	
	ANodeViewerActor();
	
};
