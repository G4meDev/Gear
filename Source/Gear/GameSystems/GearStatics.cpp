// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/GearStatics.h"

ERoadModuleTraceResult UGearStatics::TraceRoadModule(TSubclassOf<class AGearRoadModule> RoadModuleClass, const FTransform& SocketTransform)
{
	

	return ERoadModuleTraceResult::NotColliding;
}