// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

AEnemyAIController::AEnemyAIController() {
	PrimaryActorTick.bCanEverTick = true;

	// Set AI perception
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 600.0f;
	SightConfig->LoseSightRadius = 700.0f;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

}

void AEnemyAIController::BeginPlay() {
	Super::BeginPlay();
	//GetAIPerceptionComponent()->OnPerceptionUpdate.AddDynamic(this, &AEnemyAIController::SaySomething);
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::SaySomething);
}

void AEnemyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AEnemyAIController::SaySomething(AActor* actor, FAIStimulus stimulus) {
	GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Yellow, TEXT("I see you!"));
}
