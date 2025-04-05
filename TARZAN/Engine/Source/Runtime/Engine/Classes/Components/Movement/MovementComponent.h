#pragma once
#include "Components/ActorComponent.h"


class UMovementComponent: public UActorComponent
{

    DECLARE_CLASS(UMovementComponent, UActorComponent)
    
public:
    UMovementComponent* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;
    
    UMovementComponent();
    UMovementComponent(const UMovementComponent& Other);
    
};
