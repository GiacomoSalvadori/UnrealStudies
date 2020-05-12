// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "CoverActor.generated.h"

UCLASS()
class UNREALSTUDIES_API ACoverActor : public AActor
{
	GENERATED_BODY()
	
private:
//#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"

	UFUNCTION()
	void OnCompBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCompEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Returns true if the socket is close to the player */
	float DistanceFromPlayer(FName SocketName);

	/** Determines the movement direction and the facing direction of the player */
	void DetermineMovementDirection(FVector& MovementDirection, FRotator& FacingDirection);

	/** Returns the name of the nearby socket */
	FName GetNearbySocket();
	
protected:

	/** The box component that informs the player if he's able to take cover or not */
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComp;

public:
	// Sets default values for this actor's properties
	ACoverActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offset")
	FVector BoxCompOffset = FVector(50.0f, 50.0f, 0.0f);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* SM;

	/** Retrieves the movement direction and the facing rotation of the player */
	void RetrieveMovementDirectionAndFacingRotation(FVector& MovementDirection, FRotator& FacingRotation);

};
