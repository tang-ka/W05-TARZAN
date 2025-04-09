#pragma once
#include "Components/ActorComponent.h"


class UMovementComponent: public UActorComponent
{

    DECLARE_CLASS(UMovementComponent, UActorComponent)
    
public:
    UMovementComponent();
    UMovementComponent(const UMovementComponent& Other);
    
    UMovementComponent* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;

    void GetProperties(TMap<FString, FString>& OutProperties) const override;
    
    void SetProperties(const TMap<FString, FString>& InProperties) override;

    float GetSpeed() const { return Speed; }
    void SetSpeed(float NewSpeed) { Speed = NewSpeed; }
    void AddSpeed(float DeltaSpeed) { Speed += DeltaSpeed; }
    
    FVector GetVelocity() const { return Velocity; }
    void SetVelocity(const FVector& NewVelocity) { Velocity = NewVelocity; }
    void AddVelocity(const FVector& DeltaVelocity) { Velocity = Velocity + DeltaVelocity; }


protected:
    float Speed = 1.f;
    FVector Velocity = FVector(1.f, 0.f, 0.f); // 초기 속도 설정
    
};
