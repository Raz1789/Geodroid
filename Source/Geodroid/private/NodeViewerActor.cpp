// Fill out your copyright notice in the Description page of Project Settings.

#include "NodeViewerActor.h"
#include "Components/SphereComponent.h"


///***********************************************************************************************************///
///                                               CONSTRUCTOR
///***********************************************************************************************************////
ANodeViewerActor::ANodeViewerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereMesh = CreateDefaultSubobject<USphereComponent>(TEXT("VisibleMesh"));
	SphereMesh->InitSphereRadius(5.0f);
	RootComponent = SphereMesh;
	PrimaryActorTick.bCanEverTick = false;
}

