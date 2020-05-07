// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSlot.h"


FWeaponSlot::FWeaponSlot()
{
	this->WeaponMesh = NULL;
	this->Range = 10000.0f;
	this->Offset = FVector(50.0f, 0.0f, 0.0f);
}
