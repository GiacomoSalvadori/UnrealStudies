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
	GEngine->AddOnScreenDebugMessage(-1, 2.2f, FColor::Green, TEXT("Things happening "));

	for (auto& Actor : UpdateActors) {
		
		//FActorPerceptionBlueprintInfo BlueprintInfo = PerceptionComponent->GetActorInfo();
		const FActorPerceptionInfo* ActorInfo = PerceptionComponent->GetActorInfo(*Actor);
		//PerceptionComponent->GetActorsPerception(Actor, BlueprintInfo);
		
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
	GEngine->AddOnScreenDebugMessage(-1, 2.2f, FColor::Green, TEXT("Things happening"));
	DetectPlayer();
}

