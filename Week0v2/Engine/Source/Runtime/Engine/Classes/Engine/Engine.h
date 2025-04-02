#pragma once
#include "UObject/Object.h"

struct FWorldContext
{
    
};

class UEngine : UObject
{
public:
    virtual void Tick(float deltaSceconds);

protected:
    TArray<FWorldContext> worldContexts;

public:
    TArray<FWorldContext>& GetWorldContexts() { return worldContexts; }
    
};
