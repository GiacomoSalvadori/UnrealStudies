// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(40.0f);
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	

	/*
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WeaponVisualAsset(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SM_AR4_X.SM_AR4_X"));
	if (WeaponVisualAsset.Succeeded())
	{
		WeaponMesh->SetStaticMesh(WeaponVisualAsset.Object);
		//WeaponMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		//WeaponMesh->SetWorldScale3D(FVector(0.8f));
	}
	*/
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::OnConstruction(const FTransform & Transform){
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

