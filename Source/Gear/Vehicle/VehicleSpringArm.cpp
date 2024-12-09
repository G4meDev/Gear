// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/VehicleSpringArm.h"

void UVehicleSpringArm::TeleportToDesireLocation()
{
	UpdateDesiredArmLocation(bDoCollisionTest, false, false, 0.0f);
}