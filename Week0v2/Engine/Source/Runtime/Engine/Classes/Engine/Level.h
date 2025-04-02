#pragma once
#include "Container/Set.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class AActor;
class ULevel : public UObject
{
    DECLARE_CLASS(ULevel, UObject)
public:
    ULevel();
    ~ULevel();
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects() override;
    virtual void PostDuplicate() override;
private:
    TSet<AActor*> Actors;

public:
    TSet<AActor*>& GetActors() { return Actors; }
    TArray<AActor*> PendingBeginPlayActors;
};
