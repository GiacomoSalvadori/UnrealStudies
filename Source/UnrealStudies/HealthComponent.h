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

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FHealtDelegate OnGetDamage;

	UPROPERTY(BlueprintAssignable)
	FHealtDelegate OnHealtToZero; // Called when Health values is <= 0
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healt: variables")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healt: variables")
	bool bAutoRecover = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healt: variables")
	float RecoverQuantity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healt: variables")
	float RecoverQuantityTime = 02.f;

	void GetDamage(float Amount);

	void IncrementMaxHealth(float Amount);

	void Healing(float Amount);

	void AutoRecoverHealth();

	UFUNCTION(BlueprintCallable, Category = "Health")
	float HealthPercentage();

};
