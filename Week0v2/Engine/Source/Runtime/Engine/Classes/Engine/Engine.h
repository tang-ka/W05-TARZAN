#pragma once
#include "Container/Array.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

struct FWorldContext
{
    
};

class UEngine : public UObject
{
    DECLARE_CLASS(UEngine, UObject)

public:
    UEngine();
    ~UEngine();
    virtual void Tick(float deltaSceconds);

protected:
    TArray<FWorldContext> worldContexts;

public:
    TArray<FWorldContext>& GetWorldContexts() { return worldContexts; }
    
};
