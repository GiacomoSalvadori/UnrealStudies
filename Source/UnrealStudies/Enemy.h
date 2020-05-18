// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "WeaponSlot.h"
#include "EnemyPath.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStateEnemy);
UCLASS()
class UNREALSTUDIES_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float AimOffset = 60.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FWeaponSlot WeaponSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	AEnemyPath* PathToPatrol;
		

private:
	

protected:
	
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void FireWithWeapon();

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void FireWithSphereSweep();

	UFUNCTION(BlueprintCallable, Category = "Cover")
	void CrouchMe();

	UFUNCTION(BlueprintCallable, Category = "Cover")
	void UncrouchMe();

	UFUNCTION(BlueprintCallable, Category = "Cover")
	void AimIn();

	UFUNCTION(BlueprintCallable, Category = "Cover")
	void AimOut();

	UPROPERTY(BlueprintAssignable)
	FGameStateEnemy OnCharacterLanding;

	UPROPERTY(BlueprintAssignable)
	FGameStateEnemy OnCharacterJumping;

	UPROPERTY(BlueprintAssignable)
	FGameStateEnemy OnCharacterCrouch;

	UPROPERTY(BlueprintAssignable)
	FGameStateEnemy OnCharacterUncrouch;

	UPROPERTY(BlueprintAssignable)
	FGameStateEnemy OnCharacterAim;

	UPROPERTY(BlueprintAssignable)
	FGameStateEnemy OnCharacterStopAim;

	UPROPERTY(BlueprintAssignable)
	FGameStateEnemy OnCharacterStartReload;

	UPROPERTY(BlueprintAssignable)
	FGameStateEnemy OnCharacterTraceLine;
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	FORCEINLINE class UHealthComponent* GetHealthComponent() const { return HealthComponent; }
};
