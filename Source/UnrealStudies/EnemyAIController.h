// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSTUDIES_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI: behaviour tree")
	UBehaviorTree* BehaviourTree;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void StopAI();

	UFUNCTION()
	void DetectPlayer();

protected:
	virtual void BeginPlay() override;

private:
	UAISenseConfig_Sight* SightConfig;

};
