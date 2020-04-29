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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

void ATP_ThirdPersonCharacter::BeginPlay() {
	Super::BeginPlay();

	if (MovementCurve && OffsetCurve) {
		GEngine->AddOnScreenDebugMessage(-1, 2.2f, FColor::Green, TEXT("On begin bind ufucntion"));

		FOnTimelineFloat ProgressFunctionLength;
		ProgressFunctionLength.BindUFunction(this, "HandleProgressArmLength");
		AimTimeline.AddInterpFloat(MovementCurve, ProgressFunctionLength);

		FOnTimelineVector ProgressFunctionOffset;
		ProgressFunctionOffset.BindUFunction(this, "HandleProgressCameraOffset");
		AimTimeline.AddInterpVector(OffsetCurve, ProgressFunctionOffset);

	}
}

void ATP_ThirdPersonCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	AimTimeline.TickTimeline(DeltaTime);
}

void ATP_ThirdPersonCharacter::HandleProgressArmLength(float Length) {
	CameraBoom->TargetArmLength = Length;
}

void ATP_ThirdPersonCharacter::HandleProgressCameraOffset(FVector Offset) {
	CameraBoom->SocketOffset = Offset;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATP_ThirdPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATP_ThirdPersonCharacter::JumpCharacter);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATP_ThirdPersonCharacter::StopJumpingCharacter);
	
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATP_ThirdPersonCharacter::CrouchCharacter);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATP_ThirdPersonCharacter::StopCrouchCharcter);
	
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATP_ThirdPersonCharacter::AimIn);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATP_ThirdPersonCharacter::AimOut);

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
	AimTimeline.Play();
	OnCharacterAim.Broadcast();
}

void ATP_ThirdPersonCharacter::AimOut(){
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("Aim Out"));
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
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
	if ((Controller != NULL) && (Value != 0.0f))
	{
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
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
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
		OnCharacterCrouch.Broadcast();
	}
}

void ATP_ThirdPersonCharacter::StopCrouchCharcter() {
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("Uncrouch"));
	UnCrouch();
	OnCharacterUncrouch.Broadcast();
}

void ATP_ThirdPersonCharacter::Landed(const FHitResult& Hit) {
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("landed"));
	OnCharacterLanding.Broadcast();
}

void ATP_ThirdPersonCharacter::OnJumped_Implementation() {
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("On jumped"));
	OnCharacterJumping.Broadcast();
}
