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
	FVector SocketLocation = SM->GetSocketLocation(SocketName);
	FVector PlayerLocation = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	float distance = FVector::Distance(SocketLocation, PlayerLocation);

	return distance;
}

FName ACoverActor::GetNearbySocket()
{
	const FName AvailableSockets[12] =
	{
		FName("ForwardSocket_1"),
		FName("ForwardSocket_2"),
		FName("ForwardSocket_3"),
		FName("BackwardSocket_1"),
		FName("BackwardSocket_2"),
		FName("BackwardSocket_3"),
		FName("RightSocket_1"),
		FName("RightSocket_2"),
		FName("RightSocket_3"),
		FName("LeftSocket_1"),
		FName("LeftSocket_2"),
		FName("LeftSocket_3")
	};

	FName NearestSocket = AvailableSockets[0];
	
	float MinDistance = INFINITY;
	//Find the socket that is close to the character
	for (uint8 SocketPtr = 0; SocketPtr < 12; SocketPtr++) {
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
	
	//Determine the movement and facing direction of the player, based on the described logic
	//The way that we're deciding the facing direction is similar to the way we've decided
	//the movement direction
	FRotator FacingRot = GetActorRotation();

	if (NearbySocket.IsEqual("ForwardSocket_1") || NearbySocket.IsEqual("ForwardSocket_2") || NearbySocket.IsEqual("ForwardSocket_3")) {
		GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Green, TEXT("socket forward"));
		MovementDirection = -GetActorRightVector();
		FacingDirection = FacingRot + FRotator(0, 180, 0);;
	}
	else if (NearbySocket.IsEqual("BackwardSocket_1") || NearbySocket.IsEqual("BackwardSocket_2") || NearbySocket.IsEqual("BackwardSocket_3")) {
		GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Green, TEXT("socket backward"));
		MovementDirection = GetActorRightVector();
		FacingDirection = FacingRot + FRotator(0, 0, 0);
	}
	else if (NearbySocket.IsEqual("RightSocket_1") || NearbySocket.IsEqual("RightSocket_2") || NearbySocket.IsEqual("RightSocket_3")) {
		GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Green, TEXT("socket right"));
		MovementDirection = GetActorForwardVector();
		FacingDirection = FacingRot + FRotator(0, -90, 0);
	}
	else { //LeftSocket
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
		
		FVector ActorMeshScale = SM->GetComponentScale();
		BoxExtent /= ActorMeshScale;
		BoxExtent.X += BoxCompOffset.X / ActorMeshScale.X;
		BoxExtent.Y += BoxCompOffset.Y / ActorMeshScale.Y;
		BoxExtent.Z += BoxCompOffset.Z / ActorMeshScale.Z;
		
		BoxComp->SetBoxExtent(BoxExtent);
	}
}

void ACoverActor::RetrieveMovementDirectionAndFacingRotation(FVector& MovementDirection, FRotator& FacingRotation)
{
	DetermineMovementDirection(MovementDirection, FacingRotation);
}

FVector ACoverActor::GetPositionHiddenFromPlayer() {
	FName NearbySocket = GetNearbySocket();
	FName OppositeSocket = GetOppositeSocket(NearbySocket);

	FVector SocketLocation = SM->GetSocketLocation(OppositeSocket);

	return SocketLocation;
}

FName ACoverActor::GetOppositeSocket(FName Socket) {
	FName OppositeSocket = FName("ForwardSocket_1");

	if (Socket.IsEqual("ForwardSocket_1"))
		OppositeSocket = FName("BackwardSocket_1");

	if(Socket.IsEqual("ForwardSocket_2"))
		OppositeSocket = FName("BackwardSocket_1");

	if (Socket.IsEqual("ForwardSocket_3"))
		OppositeSocket = FName("BackwardSocket_1");

	if (Socket.IsEqual("BackwardSocket_1"))
		OppositeSocket = FName("ForwardSocket_1");

	if (Socket.IsEqual("BackwardSocket_2"))
		OppositeSocket = FName("ForwardSocket_1");

	if (Socket.IsEqual("BackwardSocket_3"))
		OppositeSocket = FName("ForwardSocket_1");

	if (Socket.IsEqual("RightSocket_1"))
		OppositeSocket = FName("LeftSocket_1");

	if (Socket.IsEqual("RightSocket_2"))
		OppositeSocket = FName("LeftSocket_1");

	if (Socket.IsEqual("RightSocket_3"))
		OppositeSocket = FName("LeftSocket_1");

	if (Socket.IsEqual("LeftSocket_1"))
		OppositeSocket = FName("RightSocket_1");

	if (Socket.IsEqual("LeftSocket_2"))
		OppositeSocket = FName("RightSocket_1");

	if (Socket.IsEqual("LeftSocket_3"))
		OppositeSocket = FName("RightSocket_1");

	return OppositeSocket;
}
