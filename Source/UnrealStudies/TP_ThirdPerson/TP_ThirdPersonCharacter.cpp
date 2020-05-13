// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TP_ThirdPersonCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"


//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ATP_ThirdPersonCharacter::ATP_ThirdPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->CrouchedHalfHeight = 52.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	//Create a child actor component
	//WeaponComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponComponent"));
	//WeaponComponent->SetupAttachment(RootComponent);
	//UWorld* w = GetWorld();
	
	// Add a mesh for the weapon
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "hand_rSocket");
	
	// Add Stimuli Source
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimuli Source"));

	//Add component for Health management
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	//Set active weapon index
	ActiveWeapon = 0;
	ActualEight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	IsAiming = false;
}

void ATP_ThirdPersonCharacter::BeginPlay() {
	Super::BeginPlay();

	MagBullets = Arsenal[ActiveWeapon].MagCapacity;
	FireTime = 0.0f;
	FVector WeaponLocation = GetMesh()->GetSocketLocation("hand_rSocket");
	FRotator WeaponRotaion = GetMesh()->GetSocketRotation("hand_rSocket");
	FActorSpawnParameters SpawnParams;
	//AWeapon* WeaponObj = GetWorld()->SpawnActor<AWeapon>(AWeapon::StaticClass(), WeaponLocation, WeaponRotaion, SpawnParams);
	//WeaponObj->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform, "hand_rSocket");
	//WeaponObj->Attachpa(GetMesh(), WeaponLocation);

	MaxSpeedWalkingOrig = GetCharacterMovement()->MaxWalkSpeed;
	if (MovementCurve && OffsetCurve) {
		FOnTimelineFloat ProgressFunctionLength;
		ProgressFunctionLength.BindUFunction(this, "HandleProgressArmLength");
		AimTimeline.AddInterpFloat(MovementCurve, ProgressFunctionLength);

		FOnTimelineVector ProgressFunctionOffset;
		ProgressFunctionOffset.BindUFunction(this, "HandleProgressCameraOffset");
		AimTimeline.AddInterpVector(OffsetCurve, ProgressFunctionOffset);

	}

	if (CrouchCurve) {
		FOnTimelineFloat ProgressFunctionCrouch;
		ProgressFunctionCrouch.BindUFunction(this, "HandleProgressCrouch");
		CrouchTimeline.AddInterpFloat(CrouchCurve, ProgressFunctionCrouch);
	}
}

void ATP_ThirdPersonCharacter::OnConstruction(const FTransform & Transform)
{
	/*
	WeaponComponent = NewObject<UChildActorComponent>(this);
	//UChildActorComponent* NewComp1 = NewObject<UChildActorComponent>(this);

	WeaponComponent->bEditableWhenInherited = true;
	//WeaponComponent->SetupAttachment(GetMesh(), "hand_rSocket");
	WeaponComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "hand_rSocket");
	WeaponComponent->RegisterComponent();
	if (Arsenal.Num() > 0) {
		WeaponComponent->SetChildActorClass(Arsenal[0]);
		
	} else {
		WeaponComponent->SetChildActorClass(AWeapon::StaticClass());
	}
	
	WeaponComponent->CreateChildActor();

	*/

	if (Arsenal.Num() > 0) {
		WeaponMesh->SetStaticMesh(Arsenal[0].WeaponMesh);
	}
}


void ATP_ThirdPersonCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	AimTimeline.TickTimeline(DeltaTime);
	CrouchTimeline.TickTimeline(DeltaTime);
	
	AutomaticFire(DeltaTime);
}

void ATP_ThirdPersonCharacter::HandleProgressArmLength(float Length) {
	CameraBoom->TargetArmLength = Length;
}

void ATP_ThirdPersonCharacter::HandleProgressCameraOffset(FVector Offset) {
	CameraBoom->SocketOffset = Offset;
}

void ATP_ThirdPersonCharacter::HandleProgressCrouch(float Height) {
	ActualEight = Height;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATP_ThirdPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATP_ThirdPersonCharacter::JumpCharacter);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATP_ThirdPersonCharacter::StopJumpingCharacter);
	/*
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATP_ThirdPersonCharacter::CrouchCharacter);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATP_ThirdPersonCharacter::StopCrouchCharacter);
	*/

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATP_ThirdPersonCharacter::AimIn);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATP_ThirdPersonCharacter::AimOut);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATP_ThirdPersonCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATP_ThirdPersonCharacter::StopFire);

	PlayerInputComponent->BindAction("Cover", IE_Pressed, this, &ATP_ThirdPersonCharacter::ToggleCover);
	
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATP_ThirdPersonCharacter::ReloadWeapon);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATP_ThirdPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATP_ThirdPersonCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATP_ThirdPersonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATP_ThirdPersonCharacter::LookUpAtRate);
}

void ATP_ThirdPersonCharacter::AimIn(){
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("Aim In"));
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeedAiming;
	IsAiming = true;
	if (bIsInCover) {
		GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("Aim from cover"));
		StopCrouchCharacter();
	}
	AimTimeline.Play();
	OnCharacterAim.Broadcast();
}

void ATP_ThirdPersonCharacter::AimOut(){
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("Aim Out"));
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeedWalkingOrig;
	IsAiming = false;
	if (bIsInCover) {
		GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("Stop aim from cover"));
		CrouchCharacter();
	}
	AimTimeline.Reverse();
	OnCharacterStopAim.Broadcast();
}

void ATP_ThirdPersonCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATP_ThirdPersonCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ATP_ThirdPersonCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) & !bIsInCover) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATP_ThirdPersonCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) ) {
		if (!bIsInCover) {
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		} else {
			//Move according to the cover actor's position
			AddMovementInput(CoverDirectionMovement, Value);
		}
	}
}


void ATP_ThirdPersonCharacter::JumpCharacter() {
	if (CanJump()) {
		Jump();
		GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("jumping"));
	}
}

void ATP_ThirdPersonCharacter::StopJumpingCharacter() {
	StopJumping();
}

void ATP_ThirdPersonCharacter::CrouchCharacter() {
	
	if (CanCrouch()) {
		GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("Crouch in"));
		Crouch();
		CrouchTimeline.Play();
		OnCharacterCrouch.Broadcast();
	}
}

void ATP_ThirdPersonCharacter::StopCrouchCharacter() {
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("Uncrouch"));
	if (GetCharacterMovement()->IsCrouching()) {
		UnCrouch();
		CrouchTimeline.Reverse();
		OnCharacterUncrouch.Broadcast();
	}
}

void ATP_ThirdPersonCharacter::Landed(const FHitResult& Hit) {
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("landed"));
	OnCharacterLanding.Broadcast();
}

void ATP_ThirdPersonCharacter::OnJumped_Implementation() {
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("On jumped"));
	OnCharacterJumping.Broadcast();
}

void ATP_ThirdPersonCharacter::FireFromWeapon() {
	FCollisionQueryParams Params;
	// Ignore the player's pawn
	Params.AddIgnoredActor(this->GetParentActor());

	// The hit result gets populated by the line trace
	FHitResult Hit;

	float WeaponRange = Arsenal[ActiveWeapon].Range;

	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + (FollowCamera->GetComponentRotation().Vector() * WeaponRange);

	if (!IsAiming) {
		float WeaponOffset = Arsenal[ActiveWeapon].Offset;
		GEngine->AddOnScreenDebugMessage(-1, 5.2f, FColor::Emerald, TEXT("Not aiming!"));
		Start = WeaponMesh->GetComponentLocation() + (WeaponMesh->GetForwardVector() * WeaponOffset);
		End = Start + (WeaponMesh->GetComponentRotation().Vector() * WeaponRange);
	}

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params);

	if (bHit) {
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3.0f);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Arsenal[ActiveWeapon].HitEFX, Hit.ImpactPoint);
		AEnemy* HitActor = Cast<AEnemy>(Hit.Actor.Get());

		if (HitActor) {
			GEngine->AddOnScreenDebugMessage(-1, 5.2f, FColor::Green, TEXT("Hit! " + HitActor->GetName()));
			HitActor->GetHealthComponent()->GetDamage(Arsenal[ActiveWeapon].Damage);
			HitActor->Destroy();
		}
	}
}

void ATP_ThirdPersonCharacter::AutomaticFire(float DeltaTime) {
	if (Arsenal[ActiveWeapon].IsAutomatic && bIsFiring) {
		FireTime += DeltaTime;
		if (FireTime >= Arsenal[ActiveWeapon].Rate) {
			FireTime = 0.0f;
			if (MagBullets > 0) {
				FireFromWeapon();
				MagBullets--;
			} else {
				StopFire();
				ReloadWeapon();
			}
		}
	}
}

void ATP_ThirdPersonCharacter::Fire() {
	if (!bIsReloading) {
		bIsFiring = true;
		FireTime = 0.0f;
		if (MagBullets > 0) {
			FireFromWeapon();
			MagBullets--;
		} else {
			StopFire();
			ReloadWeapon();
		}
	}
}

void ATP_ThirdPersonCharacter::StopFire() {
	bIsFiring = false;
	FireTime = 0.0f;
}

void ATP_ThirdPersonCharacter::SetCanTakeCover(bool CanTakeCover, ACoverActor* CoverActor) {
	if (!CanTakeCover && bIsInCover) {
		ToggleCover();
	}

	bCanTakeCover = CanTakeCover;
	Cover = CoverActor;
}

void ATP_ThirdPersonCharacter::ToggleCover()
{
	if (bCanTakeCover) {
		bIsInCover = !bIsInCover;
		// DEBUG message
		if (bIsInCover) {
			GEngine->AddOnScreenDebugMessage(-1, 2.2f, FColor::Green, TEXT("Cover!"));
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 2.2f, FColor::Green, TEXT("Not Cover!"));
		}
		
		if (bIsInCover && Cover) {
			
			GetCharacterMovement()->bOrientRotationToMovement = false;

			FRotator CoverRotation;
			Cover->RetrieveMovementDirectionAndFacingRotation(CoverDirectionMovement, CoverRotation);
			SetActorRotation(CoverRotation);
			CrouchCharacter();
		} else {
			GetCharacterMovement()->bOrientRotationToMovement = true;
			StopCrouchCharacter();
		}
	}
}

void ATP_ThirdPersonCharacter::ReloadWeapon() {
	GEngine->AddOnScreenDebugMessage(-1, 5.2f, FColor::Red, TEXT("Start Reload!"));
	bIsReloading = true;
	OnCharacterStartReload.Broadcast();
}

void ATP_ThirdPersonCharacter::EndReload() {
	GEngine->AddOnScreenDebugMessage(-1, 5.2f, FColor::Orange, TEXT("End Reload!"));
	bIsReloading = false;
	MagBullets = Arsenal[ActiveWeapon].MagCapacity;
}