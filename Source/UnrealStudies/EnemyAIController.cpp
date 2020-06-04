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
	SightConfig->SightRadius = 3000.0f;
	SightConfig->LoseSightRadius = 3500.0f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->PeripheralVisionAngleDegrees = 45.0f;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 1600.0f;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	HearingConfig->HearingRange = 3000.0f;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AEnemyAIController::BeginPlay() {
	Super::BeginPlay();
	
	RunBehaviorTree(BehaviourTree);
	// Inscribe to delegate to stop behaviour tree when the pawn die
	AEnemy* ControlledPawn = Cast<AEnemy>(GetPawn());

	if (ControlledPawn) {
		ControlledPawn->HealthComponent->OnHealtToZero.AddDynamic(this, &AEnemyAIController::StopAI);
		// Detect player if hit by gun
		ControlledPawn->HealthComponent->OnGetDamage.AddDynamic(this, &AEnemyAIController::DetectPlayer);
		GetBlackboardComponent()->SetValueAsFloat("OriginalWalkSpeed", ControlledPawn->GetCharacterMovement()->MaxWalkSpeed);
	}
	
	PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdate_SenseManagement);
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

void AEnemyAIController::OnPerceptionUpdate_SenseManagement(const TArray<AActor*>& UpdateActors) {
	for (auto& Actor : UpdateActors) {
		const FActorPerceptionInfo* ActorInfo = PerceptionComponent->GetActorInfo(*Actor);
		
		FAISenseID SightID = SightConfig->GetSenseID();
		if (ActorInfo->LastSensedStimuli.IsValidIndex(SightID)) {
			if (ActorInfo->LastSensedStimuli[SightID].WasSuccessfullySensed()) {
				GEngine->AddOnScreenDebugMessage(-1, 2.2f, FColor::Black, TEXT("Actor-> " + Actor->GetFullName()));
				ManageSight();
			}
		}
	}
}

void AEnemyAIController::ManageSight(){
	DetectPlayer();
}

