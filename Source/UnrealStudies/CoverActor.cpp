// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverActor.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "Math/Vector.h"

void ACoverActor::OnCompBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ATP_ThirdPersonCharacter>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.2f, FColor::Green, TEXT("CAN cover!"));
		ATP_ThirdPersonCharacter* Char = Cast<ATP_ThirdPersonCharacter>(OtherActor);
		Char->SetCanTakeCover(true, this);
	}
}

void ACoverActor::OnCompEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<ATP_ThirdPersonCharacter>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.2f, FColor::Green, TEXT("CAN not cover!"));
		//Inform the player that he isn't able to take cover
		ATP_ThirdPersonCharacter* Char = Cast<ATP_ThirdPersonCharacter>(OtherActor);
		Char->SetCanTakeCover(false, nullptr);
	}
}

float ACoverActor::DistanceFromPlayer(FName SocketName)
{
	/*
	//Perform a raycast in order to determine if the player is 
	//near the given socket
	TArray<FHitResult> HitResults;

	const FVector StartLocation = SM->GetSocketLocation(SocketName);

	const FVector EndLocation = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();

	FCollisionShape Shape;
	Shape.ShapeType = ECollisionShape::Line;

	GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat(), ECC_WorldDynamic, Shape);

	//If our raycast contains the character inside its hit result
	//the character can take cover in the side that this socket represents
	return HitResults.ContainsByPredicate([&](FHitResult hitResult)
	{
		AActor* HitActor = hitResult.GetActor();
		return HitActor && HitActor->IsA<ATP_ThirdPersonCharacter>();
	});
	*/

	FVector SocketLocation = SM->GetSocketLocation(SocketName);
	FVector PlayerLocation = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	float distance = FVector::Distance(SocketLocation, PlayerLocation);

	return distance;
}

FName ACoverActor::GetNearbySocket()
{
	const FName AvailableSockets[4] =
	{
		FName("ForwardSocket"),
		FName("BackwardSocket"),
		FName("RightSocket"),
		FName("LeftSocket")
	};

	FName NearestSocket = AvailableSockets[0];
	float MinDistance = 10000.0f;
	//Find the socket that is close to the character
	for (uint8 SocketPtr = 0; SocketPtr < 4; SocketPtr++) {
		float Distance = DistanceFromPlayer(AvailableSockets[SocketPtr]);
		if (Distance < MinDistance) {
			NearestSocket = AvailableSockets[SocketPtr];
			MinDistance = Distance;
		}
	}

	//If something goes terribly wrong we're going to get the forward wall
	return NearestSocket;
}

void ACoverActor::DetermineMovementDirection(FVector& MovementDirection, FRotator& FacingDirection)
{
	FName NearbySocket = GetNearbySocket();
	AActor* Char = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	//Determine the movement and facing direction of the player, based on the described logic
	//The way that we're deciding the facing direction is similar to the way we've decided
	//the movement direction
	FRotator FacingRot = GetActorRotation();
	if (NearbySocket.IsEqual("ForwardSocket")) {
		GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Green, TEXT("socket forward"));
		MovementDirection = -GetActorRightVector();
		FacingDirection = FacingRot + FRotator(0, 180, 0);;
	} else if (NearbySocket.IsEqual("BackwardSocket")) {
		GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Green, TEXT("socket backward"));
		MovementDirection = GetActorRightVector();
		FacingDirection = FacingRot + FRotator(0, 0, 0);
	} else if (NearbySocket.IsEqual("RightSocket")) {
		GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Green, TEXT("socket right"));
		MovementDirection = GetActorForwardVector();
		FacingDirection = FacingRot + FRotator(0, -90, 0);
	} else { //LeftSocket
		GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Green, TEXT("socket left"));
		MovementDirection = -GetActorForwardVector();
		FacingDirection = FacingRot + FRotator(0, 90.f, 0);
	}
}

// Sets default values
ACoverActor::ACoverActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init. components
	SM = CreateDefaultSubobject<UStaticMeshComponent>(FName("SM"));
	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComp"));

	SetRootComponent(SM);

	BoxComp->SetupAttachment(SM);
}

// Called when the game starts or when spawned
void ACoverActor::BeginPlay()
{
	Super::BeginPlay();

	if (BoxComp) {
		//Register overlap events
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACoverActor::OnCompBeginOverlap);
		BoxComp->OnComponentEndOverlap.AddDynamic(this, &ACoverActor::OnCompEndOverlap);
		FVector BoxExtent = BoxComp->GetScaledBoxExtent();
		GEngine->AddOnScreenDebugMessage(-12, 20.0f, FColor::Purple, FString::Printf(TEXT("Dim: %f - %f - %f"), BoxExtent.X, BoxExtent.Y, BoxExtent.Z));
		FVector ActorMeshScale = SM->GetComponentScale();
		BoxExtent /= ActorMeshScale;
		BoxExtent.X += BoxCompOffset.X / ActorMeshScale.X;
		BoxExtent.Y += BoxCompOffset.Y / ActorMeshScale.Y;
		BoxExtent.Z += BoxCompOffset.Z / ActorMeshScale.Z;
		
		GEngine->AddOnScreenDebugMessage(-10, 20.0f, FColor::Purple, FString::Printf(TEXT("Dim: %f - %f - %f"), BoxExtent.X, BoxExtent.Y, BoxExtent.Z));
		BoxComp->SetBoxExtent(BoxExtent);
	}
}

void ACoverActor::RetrieveMovementDirectionAndFacingRotation(FVector& MovementDirection, FRotator& FacingRotation)
{
	DetermineMovementDirection(MovementDirection, FacingRotation);
}
