#pragma once

#include "Engine.h"
#include "Throwable.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FThrowable
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh *WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MaxCapacity;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageRadius;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystem* HitEFX;

	FThrowable();
};