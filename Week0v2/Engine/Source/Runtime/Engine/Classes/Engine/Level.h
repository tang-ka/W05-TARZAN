#pragma once
#include "GameFramework/Actor.h"
#include "UObject/Object.h"

class ULevel : UObject
{
public:
    TArray<AActor*> Actors;
        
};
