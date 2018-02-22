// Fill out your copyright notice in the Description page of Project Settings.

#include "PointerProtection.h"

bool UPointerProtection::CheckAndLog(void* Pointer, FString PointerName)
{
	if (Pointer)
	{
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s is a null pointer"), *PointerName);
		return false;
	}
}
