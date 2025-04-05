#pragma once

#include "MovementComponent.h"
#include "Math/Vector.h"

class URotatingMovementComponent : public UMovementComponent
{
    DECLARE_CLASS(URotatingMovementComponent, UMovementComponent)

public:

    
public:
    URotatingMovementComponent();
    URotatingMovementComponent(const URotatingMovementComponent& Other);

    URotatingMovementComponent* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;

    void TickComponent(float DeltaTime) override;
    

private:
    // FVector RotationRate = FVector(0.f, 0.f, 1.f); // 회전 속도 설정
    // float RotationSpeed = 1.f; // 회전 속도 설정
    
};
