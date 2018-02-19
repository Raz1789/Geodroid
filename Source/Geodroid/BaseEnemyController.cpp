// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemyController.h"


void ABaseEnemyController::MoveToPosition(FVector _Position)
{
	MoveToLocation(_Position, 50.f);
}

