// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GeodroidCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGeodroidCharacter

AGeodroidCharacter::AGeodroidCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->Hand = EControllerHand::Right;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	///MODIFIED AFTER THIS POINT FROM THE ORIGINAL TEMPLATE
	PlayerGold = 100;
	PlayerMaxHealth = 100.f;
	AttackDamage = 1.0f;
	CrossHairLocation = FVector2D(0.5f, 0.5f);
	PrimaryActorTick.bCanEverTick = true;

	///Defense Structure Related variable initialization
	SelectedDefenseStructure = ESelectDefenseStructure::ESDS_Turret;
	ConstructionStatus = EConstructionStatus::ECS_NoActivity;
	StructureConstructionRange = 1200.f;

	/***************** Setting the Component Collision function **********/
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AGeodroidCharacter::OnHit);
}

void AGeodroidCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	PlayerHealth = PlayerMaxHealth;


	if (UPointerProtection::CheckAndLog(GetWorld(), "Player World Pointer"))
	{
		World = GetWorld();
	}

	if (UPointerProtection::CheckAndLog(Cast<APlayerController>(GetController()), TEXT("Player Controller")))
	{
		PlayerController = Cast<APlayerController>(GetController());
	}
	
	if (PlayerController && UPointerProtection::CheckAndLog(Cast<AGeodroidHUD>(PlayerController->GetHUD()), TEXT("Player Controller")))
	{
		PlayerHUD = Cast<AGeodroidHUD>(PlayerController->GetHUD());
	}

	if (PlayerHUD)
	{
		PlayerHUD->ReceivePopUpMessage("I'm Alive (for 3 seconds)");
	}

}

void AGeodroidCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!World) return; //TODO Remove only for debug
	///DEFENSE TOWER CONSTRUCTION

	//CharacterMapNode = UMapClass::WorldToMapNode(GetActorLocation());
	//
	//switch (ConstructionStatus)
	//{
	//case EConstructionStatus::ECS_NoActivity:
	//	DestroyStructure();
	//	break;
	//case EConstructionStatus::ECS_CheckingSite:
	//	StructurePlacement();
	//	break;
	//case EConstructionStatus::ECS_Constructing:
	//	BuildDefenseStructure();
	//	break;
	//}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//DEBUG

	//UE_LOG(LogTemp, Warning, TEXT("Node Walkable 13,0: %s"), *FString(UMapClass::IsMapNodeWalkable(13, 0) ? "true" : "false"));
	FVector StartLocation;
	FVector LookDirection;

	GetCameraDetails(StartLocation, LookDirection);

	FVector EndLocation = StartLocation + (LookDirection * StructureConstructionRange);

	DrawDebugLine(World,
				  StartLocation,
				  EndLocation,
				  FColor::Red,
				  false);
}



//////////////////////////////////////////////////////////////////////////
// Input

void AGeodroidCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGeodroidCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AGeodroidCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGeodroidCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGeodroidCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGeodroidCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGeodroidCharacter::LookUpAtRate);
	
	//Defense Structure Related Inputs
	PlayerInputComponent->BindAction("SelectTurret", IE_Pressed, this, &AGeodroidCharacter::SelectTurretForConstruction);
	PlayerInputComponent->BindAction("SelectTrap", IE_Pressed, this, &AGeodroidCharacter::SelectTrapForConstruction);
	PlayerInputComponent->BindAction("DestroyStructure", IE_Pressed, this, &AGeodroidCharacter::DestroyInit);
	PlayerInputComponent->BindAction("PlaceStructure", IE_Pressed, this, &AGeodroidCharacter::StructurePlacement);
}

void AGeodroidCharacter::OnFire()
{
	//Pointer Protection
	if (!World) return;

	// try and fire a projectile
	if (UPointerProtection::CheckAndLog(ProjectileClass, "Player Projectile Class"))
	{
		if (bUsingMotionControllers)
		{
			const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
			const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
			World->SpawnActor<AGeodroidProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
		else
		{
			AGeodroidProjectile* Projectile;

			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			ActorSpawnParams.Instigator = this;

			// spawn the projectile at the muzzle
			Projectile = World->SpawnActor<AGeodroidProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			if (UPointerProtection::CheckAndLog(Projectile, "Player Projectile"))
			{
				Projectile->SetBulletDamage(AttackDamage);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AGeodroidCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AGeodroidCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AGeodroidCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AGeodroidCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AGeodroidCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGeodroidCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGeodroidCharacter::TurnAtRate(float Rate)
{
	//Pointer Protection
	if (!World) return;

	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * World->GetDeltaSeconds());
}

void AGeodroidCharacter::LookUpAtRate(float Rate)
{
	//Pointer Protection
	if (!World) return;
	
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * World->GetDeltaSeconds());
}

bool AGeodroidCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AGeodroidCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AGeodroidCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AGeodroidCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

bool AGeodroidCharacter::DeductStructureCost(int32 AmountToBeDeducted)
{
	bool bCanBeSubracted = (PlayerGold > AmountToBeDeducted);

	if (bCanBeSubracted)
	{
		PlayerGold -= AmountToBeDeducted;
	}

	return bCanBeSubracted;
}

void AGeodroidCharacter::StructurePlacement()
{
	///Pointer Protection
	if (!World) return;

	///Set bIsConstructionFeasible = false;
	bIsConstructionFeasible = false;

	///Line Trace forward looking for Profile "Floor"
	AActor* FloorActor;

	FloorActor = FloorCheck();

	if (FloorActor)
	{
		bool bIsFloorVisible = VisibilityCheck(FloorActor);

		if (bIsFloorVisible)
		{
			///extract the FloorHitposition and do a pivot adjustment
			FVector FloorPosition = FloorActor->GetActorLocation() + FVector(200.f, 200.f, 0.f);
			///Convert the FloorPosition to MapNode
			FVector2D FloorNodeIndex = UMapClass::WorldToMapNode(FloorPosition).NodeIndex;
			///Check if FloorNode isWalkable
			bool bIsFloorNodeWalkable = UMapClass::IsMapNodeWalkable(FloorNodeIndex.X, FloorNodeIndex.Y);
			if (bIsFloorNodeWalkable)
			{
				///Checking that character is not on the SpawnLocation
				if (CharacterMapNode.NodeIndex.X != FloorNodeIndex.X && CharacterMapNode.NodeIndex.Y != FloorNodeIndex.Y)
				{
					///Set FloorNode Walkable to false
					UMapClass::SetMapNodeWalkable(FloorNodeIndex.X, FloorNodeIndex.Y, false);
					///Check if PathExist if Structure placed
					UA_Pathfinding* Pathfinding = NewObject<UA_Pathfinding>();
					bool bIsPathAvailable = Pathfinding->PathExist(FloorNodeIndex);

					if (bIsPathAvailable)
					{
						///Spawn the Structure
						SpawnStructure(FloorNodeIndex, FloorPosition);
					}
					else
					{
						PlayerHUD->ReceivePopUpMessage("You will block the path if you construct here");
						///Set FloorNode Walkable to false
						UMapClass::SetMapNodeWalkable(FloorNodeIndex.X, FloorNodeIndex.Y, true);
					}
				}
				else
				{
					PlayerHUD->ReceivePopUpMessage("You are standing on the Construction Location");
				}
			}
			else
			{
				PlayerHUD->ReceivePopUpMessage("That is not a walkable node");
			}
		}
		else
		{
			PlayerHUD->ReceivePopUpMessage("Floor is not visible");
		}
	}
	else
	{
		PlayerHUD->ReceivePopUpMessage("Structure should be placed on the floor");
	}

	if (bIsConstructionFeasible)
	{
		PlayerHUD->ReceivePopUpMessage("Structure constructed!!!");
	}

}

void AGeodroidCharacter::SpawnStructure(FVector2D &FloorNodeIndex, FVector &FloorPosition)
{
	///Create New Structure and assign to SpawnedStructure;
	if (DefenseStructuresClasses.Num() > (uint8)(SelectedDefenseStructure)) //Check if DefenseStructure Class element at checking location exist
	{
		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ActorSpawnParams.Instigator = this;

		DefenseStructuesSpawnList.Add(World->SpawnActor<ADefenseStructures>(
			DefenseStructuresClasses[(uint8)(SelectedDefenseStructure)],
			FloorPosition,
			FRotator::ZeroRotator,
			ActorSpawnParams));
		if (DefenseStructuesSpawnList[DefenseStructuesSpawnList.Num()-1])
		{
			DefenseStructuesSpawnList[DefenseStructuesSpawnList.Num() - 1]->ActivateTower();
			DefenseStructuesSpawnList[DefenseStructuesSpawnList.Num() - 1]->MaterialColor = FColor::Silver;
			///Set bIsConstructionFeasible = true;
			bIsConstructionFeasible = true;
			for (auto& Spawn : DefenseStructuesSpawnList)
			{
				UE_LOG(LogTemp, Warning, TEXT("Spawn: %s"), *Spawn->GetName());
			}
		}
		else
		{
			PlayerHUD->ReceivePopUpMessage("Structure cannot be constructed there due to collision!!!");
			///Set FloorNode Walkable back to true
			UMapClass::SetMapNodeWalkable(FloorNodeIndex.X, FloorNodeIndex.Y, true);
			bIsConstructionFeasible = false;
		}

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Classes for Defense Structure not added"));
	}
	
}

void AGeodroidCharacter::BuildDefenseStructure()
{
	
}

void AGeodroidCharacter::DestroyStructure(ADefenseStructures* Structure)
{
	if (!World) return;

	if (Structure)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroyed"));
		World->DestroyActor(Structure);
	}
}

void AGeodroidCharacter::SelectTurretForConstruction()
{
	SelectedDefenseStructure = ESelectDefenseStructure::ESDS_Turret;
}

void AGeodroidCharacter::SelectTrapForConstruction()
{
	SelectedDefenseStructure = ESelectDefenseStructure::ESDS_Trap;
}

void AGeodroidCharacter::StartCheckingSite()
{
	ConstructionStatus = EConstructionStatus::ECS_CheckingSite;
}

void AGeodroidCharacter::DestroyInit()
{

}

void AGeodroidCharacter::DebugFunction()
{
	//Pointer Protection
	if (!World) return;

	if (UPointerProtection::CheckAndLog(NodeViewerClass, "NodeViewerClass"))
	{
		//Get the Map's Max Size
		FVector2D MapMaxSize = UMapClass::GetMapMaxSize();
		//Iterate through each Node
		for (int32 X = 0; X < MapMaxSize.X; X++)
		{
			for (int32 Y = 0; Y < MapMaxSize.Y; Y++)
			{
				//Ask Gamemode for the position of the Node
				FVector position = UMapClass::GetMapNodePosition(X, Y);
				position.Z = 100.f;

				//Spawn the actor at that location
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				ANodeViewerActor* NodeViewer = World->SpawnActor<ANodeViewerActor>(NodeViewerClass, position, FRotator(0.f), SpawnParams);
			}
		}
	}
}

void AGeodroidCharacter::AddGold(int32 AmountReceived)
{
	PlayerGold += AmountReceived;
}

bool AGeodroidCharacter::SubtractGold(int32 AmountToBeDeducted)
{
	if (AmountToBeDeducted > PlayerGold)
	{
		return false;
	}
	else
	{
		PlayerGold -= AmountToBeDeducted;
		return true;
	}
}

int32 AGeodroidCharacter::GetPlayerGold() const
{
	return PlayerGold;
}

float AGeodroidCharacter::GetPlayerHealth() const
{
	return PlayerHealth / PlayerMaxHealth;
}

void AGeodroidCharacter::ApplyDamage(float Damage)
{
	PlayerHealth = std::max(0.0f, PlayerHealth - Damage);
	UE_LOG(LogTemp, Warning, TEXT("Player health: %f"), PlayerHealth);
	if (PlayerHealth <= 0 && !bIsPlayerDead)
	{
		bIsPlayerDead = true;
	}
}

bool AGeodroidCharacter::IsPlayerDead()
{
	return bIsPlayerDead;
}

void AGeodroidCharacter::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	AGeodroidProjectile* Collider = Cast<AGeodroidProjectile>(OtherActor);
	if (Collider)
	{
		ApplyDamage(Collider->GetBulletDamage());
	}

	UE_LOG(LogTemp, Warning, TEXT("Player health: %f"), PlayerHealth);
}

bool AGeodroidCharacter::VisibilityCheck(const  AActor* TargetActor)
{ 
	//Pointer Protection
	if (!World) return nullptr;

	FVector StartLocation;
	FVector LookDirection;

	GetCameraDetails(StartLocation, LookDirection);

	FVector EndLocation = StartLocation + (LookDirection * StructureConstructionRange);

	///Setting the CollisionQueryParams
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	FHitResult OutHit;

	World->LineTraceSingleByChannel(OutHit,
									StartLocation,
									EndLocation,
									ECollisionChannel::ECC_Visibility, ///Channel pertaining to Floor
									CollisionParam);

	if (OutHit.GetActor() == TargetActor)
	{
		return true;
	}

	return false;
}

AActor* AGeodroidCharacter::FloorCheck()
{
	//Pointer Protection
	if (!World) return nullptr;

	FVector StartLocation;
	FVector LookDirection;
	
	GetCameraDetails(StartLocation, LookDirection);

	FVector EndLocation = StartLocation + (LookDirection * StructureConstructionRange);

	//UE_LOG(LogTemp, Warning, TEXT("------------------------\nStart: %s, \nLookDirection: %s, \nEndLocation: %s \n--------------------------"), *StartLocation.ToString(), *LookDirection.ToString(), *EndLocation.ToString());

	///Setting the CollisionQueryParams
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	FHitResult OutHit;

	World->LineTraceSingleByChannel(OutHit,
									StartLocation,
									EndLocation,
									ECollisionChannel::ECC_GameTraceChannel2, ///Channel pertaining to Floor
									CollisionParam);
	DrawDebugLine(World,
				  StartLocation,
				  EndLocation,
				  FColor::Green,
				  false);

	return OutHit.GetActor();
}

void AGeodroidCharacter::GetCameraDetails(FVector& OutCameraLocation, FVector& OutCameraLookDirection)
{
	//Pointer Protection
	if (!PlayerController) return;
	
	//Returning the Details
	OutCameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
	OutCameraLookDirection = PlayerController->PlayerCameraManager->GetCameraRotation().Vector();
}