// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "PointSlot.generated.h"

USTRUCT(BlueprintType)
struct FPointSlot
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LocationPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationPoint;


	FPointSlot();
};