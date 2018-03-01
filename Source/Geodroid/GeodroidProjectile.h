// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeodroidProjectile.generated.h"

UCLASS(config=Game)
class AGeodroidProjectile : public AActor
{
	GENERATED_BODY()


protected:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	///MEMBER VARIABLE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Design")
	//Specify the amount of damage the projectile should cause in blueprint
	float BulletDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Design")
	//Specify the initial velocity the projectile will be fired at
	float InitialVelocity;

	///MEMEBER FUNCTIONS
	//virtual void BeginPlay() override;

public:
	AGeodroidProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	///MEMBER VARIABLES

	//Getter for the BulletDamage Variable
	float GetBulletDamage();

	//Setter for the BulletDamage Variable
	void SetBulletDamage(float _BulletDamage);
};

