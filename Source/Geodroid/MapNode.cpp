// Fill out your copyright notice in the Description page of Project Settings.

#include "MapNode.h"

void FMapNode::StructInit(FVector2D _NodeIndex, FVector _Position, bool _bWalkable)
{
	NodeIndex = _NodeIndex;
	Position = _Position;
	bWalkable = _bWalkable;
}

bool FMapNode::operator==(const FMapNode & Other) const
{
	return (NodeIndex.X == Other.NodeIndex.X) && (NodeIndex.Y == Other.NodeIndex.Y);
}


