#pragma once
#include "MovementComponent.h"
#include "Math/Vector.h"

class UProjectileMovementComponent : public UMovementComponent
{
    DECLARE_CLASS(UProjectileMovementComponent, UMovementComponent)

public:

    
public:
    UProjectileMovementComponent();
    UProjectileMovementComponent(const UProjectileMovementComponent& Other);

    UProjectileMovementComponent* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;
    
    
    
    void TickComponent(float DeltaTime) override;


private:
    
};
