// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	HealthDefaultValue = Health;
	HealthMaxValue = Health;
}


void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UHealthComponent::GetDamage(float Amount) {
	Health -= Amount;
	
	OnGetDamage.Broadcast();
	if (Health <= 0) {
		OnHealtToZero.Broadcast();
	}
}

void UHealthComponent::IncrementMaxHealth(float Amount) {
	HealthMaxValue += Amount;
}

void UHealthComponent::Healing(float Amount) {
	Health += Amount;
}

void UHealthComponent::AutoRecoverHealth() {

}

float UHealthComponent::HealthPercentage() {

	return HealthMaxValue == 0.0 ? 1.0f : (Health/HealthMaxValue);
}
