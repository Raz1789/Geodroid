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

	///Defense Structure Related variable initialization
	SelectedDefenseStructure = ESelectDefenseStructure::ESDS_Turret;
	ConstructionStatus = EConstructionStatus::ECS_NoActivity;
	bIsSiteInspectionNodeSet = false;

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
}

void AGeodroidCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	///DEFENSE TOWER CONSTRUCTION
	switch (ConstructionStatus)
	{
	case EConstructionStatus::ECS_NoActivity:
		break;
	case EConstructionStatus::ECS_CheckingSite:
		CheckFeasibilityForConstruction();
		break;
	case EConstructionStatus::ECS_Constructing:

		ConstructionStatus = EConstructionStatus::ECS_NoActivity;
		break;
	}

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

	//Place Turret
	PlayerInputComponent->BindAction("PlaceStructure", IE_Pressed, this, &AGeodroidCharacter::StartCheckingSite);
	PlayerInputComponent->BindAction("PlaceStructure",IE_Released, this, &AGeodroidCharacter::StartStructureConstruction);

	//Select the Defense Structure
	PlayerInputComponent->BindAction("SelectTurret", IE_Pressed, this, &AGeodroidCharacter::SelectTurretForConstruction);
	PlayerInputComponent->BindAction("SelectTrap", IE_Pressed, this, &AGeodroidCharacter::SelectTrapForConstruction);
}

void AGeodroidCharacter::OnFire()
{

	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
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
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileClass is null"));
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
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGeodroidCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
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

void AGeodroidCharacter::CheckFeasibilityForConstruction()
{

	FHitResult OutHit;

	//TODO Create a CancelAction button function
	//TODO Create ConstructionReset function

	///Set bIsConstructionFeasible = false;
	bIsConstructionFeasible = false;
	///Line Trace forward looking for Profile "Floor"
		
	///extract the FloorHitposition and do a pivot adjustment
	///Convert the FloorPosition to MapNode
	///Check if SiteInspectedNode is Set
		///Check if the SiteInspectedNode NOT equals to FloorNode
			///Check if FloorNode isWalkable
				///Set FloorNode Walkable to false
				///Check if PathExist if Structure placed
					///Check if PreviousSpawnStructure Pointer is not null
						///Destroy Previously Constructed DefenseStructure
					///Set SiteInspectedNode = FloorNode
					///Create New Structure and assign to PreviousSpawnedStructure;
					///Set StructureColor to Green
					///Set bIsConstructionFeasible = true;
}

void AGeodroidCharacter::BuildDefenseStructure()
{}

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

void AGeodroidCharacter::StartStructureConstruction()
{
	ConstructionStatus = EConstructionStatus::ECS_Constructing;
}

void AGeodroidCharacter::DebugFunction()
{
	if (NodeViewerClass != NULL)
	{
		UWorld* const World = GetWorld();
		if(World != NULL)
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
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GetWorld not initialized"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NodeViewer Class not initialized"));
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
{ /*
	//Pointer Protection
	UWorld* World = GetWorld(); //TODO Make All GetWorld Together
	if (!World) return false;

	FHitResult OutHit;

	///Getting the spawn Location
	FVector TargetLocation;

	///Vector to Enemy = -TurretVector from origin + EnemyVector from Origin (Shoot Offset is not required for visibility check)
	TargetLocation = -GetActorLocation() + TargetActor->GetActorLocation();
	TargetLocation.Normalize(); ///To get the Direction Vector
	FVector SpawnDirection = TargetLocation;
	TargetLocation *= 100.f; ///Scale to get a point 50cm from the start of vector
	TargetLocation += BP_Turret->GetComponentLocation(); ///Traslating the Vector to the Turret

														///Getting SpawnRotation
	FRotator SpawnRotation;
	SpawnRotation = SpawnDirection.Rotation();

	///Setting the CollisionQueryParams
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	World->LineTraceSingleByChannel(OutHit,
									TargetLocation,
									TargetLocation + SpawnDirection * (InfluenceBox.GetSphereRadius()),
									ECC_Visibility,
									CollisionParam);
	DrawDebugLine(World,
				  TargetLocation,
				  TargetLocation + SpawnDirection * (InfluenceBox.GetSphereRadius()),
				  FColor::Red,
				  false);

	if (OutHit.GetActor() == TargetEnemy)
	{
		return true;
	}
	*/
	return false;
}