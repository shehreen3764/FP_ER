// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once
#ifndef __FP_ER_H__
#define __FP_ER_H__

#include "Engine.h"
#include "CoreMinimal.h"

#define print(text) if(GEngine)GEngine->AddOnScreenDebugMessage(-1,GetWorld()->GetDeltaSeconds(),FColor::White,text)

//general log
//DECLARE_LOG_CATEGORY_EXTERN(YourLog, Log, All);

//logging during game startup
//DECLARE_LOG_CATEGORY_EXTERN(YourInit, Log, All);

//logging for the AI system
//DECLARE_LOG_CATEGORY_EXTERN(YourAI, Log, All);

//logging for critical errors that must always be addressed
//DECLARE_LOG_CATEGORY_EXTERN(YourCriticalErrors, Log, All);

#endif

