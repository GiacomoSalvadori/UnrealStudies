// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealtDelegate);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALSTUDIES_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	float HealthDefaultValue;
	float HealthMaxValue;
	float ElapsedTime;
	float TimeSinceLastDamage;
	bool bIsDamaged;

	void AutoRecoveryHealth(float DeltaTime);

	void CheckDamageTime(float DeltaTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FHealtDelegate OnGetDamage;

	UPROPERTY(BlueprintAssignable)
	FHealtDelegate OnHealtToZero; // Called when Health values is <= 0

	UPROPERTY(BlueprintAssignable)
	FHealtDelegate OnHealthRecovery; // Called every time Health is recovered
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healt: variables")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healt: variables")
	bool bAutoRecovery = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healt: variables")
	float RecoveryQuantity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healt: variables")
	float HealthRecoveryTime = 0.2f;

	/** Time to wait after receive damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healt: variables")
	float NoDamageTimeForRecovery = 2.5f;

	void GetDamage(float Amount);

	void IncrementMaxHealth(float Amount);

	void Healing(float Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	float HealthPercentage();

};
