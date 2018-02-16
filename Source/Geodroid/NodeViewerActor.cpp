// Fill out your copyright notice in the Description page of Project Settings.

#include "NodeViewerActor.h"


// Sets default values
ANodeViewerActor::ANodeViewerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereMesh = CreateDefaultSubobject<USphereComponent>(TEXT("VisibleMesh"));
	SphereMesh->InitSphereRadius(5.0f);
	RootComponent = SphereMesh;
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ANodeViewerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANodeViewerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

