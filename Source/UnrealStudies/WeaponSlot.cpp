// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSlot.h"


FWeaponSlot::FWeaponSlot()
{
	this->WeaponMesh = NULL;
	this->Range = 10000.0f;
	this->HitRadius = 50.0f;
	this->Offset = 55.0f;
	this->HitEFX = NULL;
}
