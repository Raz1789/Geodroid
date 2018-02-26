// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GeodroidHUD.generated.h"

UCLASS()
class AGeodroidHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGeodroidHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "CPP Function", meta = (DisplayName = "PopUpMessage"))
	void ReceivePopUpMessage(const FString& Message);

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

