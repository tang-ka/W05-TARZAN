#pragma once
#include "GameFramework/Actor.h"
#include "UObject/Object.h"

class ULevel : UObject
{
private:
    TSet<AActor*> Actors;

public:
    const TSet<AActor*>& GetActors() const { return Actors; }
};
