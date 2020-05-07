// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "WeaponSlot.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh *WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Offset;

	FWeaponSlot();
};
