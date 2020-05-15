// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPath.generated.h"

UCLASS()
class UNREALSTUDIES_API AEnemyPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyPath();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path Points", Meta = (MakeEditWidget = true))
	TArray<FVector> PathPoints;

private:
	int PathSense;
	int PathIndex;

protected:

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:

	virtual void Tick(float DeltaTime) override;

	void GoNextNode();

	FVector ActualPoint();

};
