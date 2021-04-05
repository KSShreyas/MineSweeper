#pragma once

#include "CoreMinimal.h"


#define MineLog(LogCat,LogType,Param1) UE_LOG(LogCat,LogType,TEXT("%s"),*FString(Param1))
