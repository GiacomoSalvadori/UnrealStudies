// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "../Weapon.h"
#include "../WeaponSlot.h"
#include "../Enemy.h"
#include "../CoverActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TP_ThirdPersonCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStateCharacter);
UCLASS(config=Game)
class ATP_ThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponMesh;

public:
	ATP_ThirdPersonCharacter();

	UPROPERTY(BlueprintAssignable)
	FGameStateCharacter OnCharacterLanding;

	UPROPERTY(BlueprintAssignable)
	FGameStateCharacter OnCharacterJumping;
	
	UPROPERTY(BlueprintAssignable)
	FGameStateCharacter OnCharacterCrouch;
	
	UPROPERTY(BlueprintAssignable)
	FGameStateCharacter OnCharacterUncrouch;

	UPROPERTY(BlueprintAssignable)
	FGameStateCharacter OnCharacterAim;

	UPROPERTY(BlueprintAssignable)
	FGameStateCharacter OnCharacterStopAim;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	
	/** Walk speed while aiming. */
	UPROPERTY(EditAnywhere, Category = "Aim")
	float MaxSpeedAiming = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aim")
	float ActualEight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* MovementCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveVector* OffsetCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* CrouchCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FWeaponSlot> Arsenal;
	//TArray<TSubclassOf<AWeapon>> Arsenal;
	//TArray<class AWeapon*> Arsenal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	UParticleSystem* HitEFX;

private:
	int ActiveWeapon;

	float MaxSpeedWalkingOrig;

	bool IsAiming;

	FTimeline AimTimeline;

	FTimeline CrouchTimeline;
	
	bool bCanTakeCover = false;
	
	bool bIsInCover = false;
	
	FVector CoverDirectionMovement;

	ACoverActor* Cover;

	UFUNCTION()
	void HandleProgressArmLength(float Length);
	
	UFUNCTION()
	void HandleProgressCameraOffset(FVector Offset);

	UFUNCTION()
	void HandleProgressCrouch(float Height);

protected:
	
	void JumpCharacter();
	void StopJumpingCharacter();

	void CrouchCharacter();
	void StopCrouchCharacter();

	void AimIn();
	void AimOut();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	virtual void OnConstruction(const FTransform& Transform) override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	

public:
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnJumped_Implementation();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/** Called when the player try to fire with weapon */
	void Fire();

	/**Enables or disables the cover mode*/
	void ToggleCover();

	/** Inform the player that he's able to take cover in the provided actor */
	void SetCanTakeCover(bool bCanTakeCover, ACoverActor* CoverActor);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

