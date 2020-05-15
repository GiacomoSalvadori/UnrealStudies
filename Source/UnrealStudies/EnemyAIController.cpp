// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Enemy.h"
#include "HealthComponent.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController() {
	PrimaryActorTick.bCanEverTick = true;

	// Set AI perception
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 600.0f;
	SightConfig->LoseSightRadius = 700.0f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AEnemyAIController::BeginPlay() {
	Super::BeginPlay();

	RunBehaviorTree(BehaviourTree);
	// Inscribe to delegate to stop behaviour tree when the pawn die
	AEnemy* ControlledPawn = Cast<AEnemy>(GetPawn());

	if (ControlledPawn) {
		ControlledPawn->HealthComponent->OnHealtToZero.AddDynamic(this, &AEnemyAIController::StopAI);
		ControlledPawn->HealthComponent->OnGetDamage.AddDynamic(this, &AEnemyAIController::DetectPlayer);
	}
}

void AEnemyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AEnemyAIController::StopAI() {
	BrainComponent->StopLogic("Death");
	AEnemy* ControlledPawn = Cast<AEnemy>(GetPawn());

	if (ControlledPawn) {
		ControlledPawn->GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		ControlledPawn->HealthComponent->OnHealtToZero.RemoveDynamic(this, &AEnemyAIController::StopAI);
		ControlledPawn->HealthComponent->OnGetDamage.RemoveDynamic(this, &AEnemyAIController::DetectPlayer);
	}
	Destroy();
}

void AEnemyAIController::DetectPlayer() {
	GetBlackboardComponent()->SetValueAsBool("SeePlayer", true);
}