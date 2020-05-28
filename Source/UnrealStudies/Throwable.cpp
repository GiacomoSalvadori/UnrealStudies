#pragma once

#include "Throwable.h"

FThrowable::FThrowable() {
	this->WeaponMesh = NULL;
	this->MaxCapacity = 5;
	this->Damage = 100.0f;
	this->DamageRadius = 1000.0f;
	this->HitEFX = NULL;
}
