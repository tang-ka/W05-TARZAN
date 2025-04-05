#include "MovementComponent.h"

#include "UObject/ObjectFactory.h"


UMovementComponent::UMovementComponent()
{
    Speed = 1.f;
    Velocity = FVector(1.f, 0.f, 0.f); // 초기 속도 설정
}

UMovementComponent::UMovementComponent(const UMovementComponent& Other)
    : UActorComponent(Other),
      Speed(Other.Speed),
      Velocity(Other.Velocity)
{
}

UMovementComponent* UMovementComponent::Duplicate() const
{
    UMovementComponent* NewComp = FObjectFactory::ConstructObjectFrom<UMovementComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    
    return NewComp;
}

void UMovementComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void UMovementComponent::PostDuplicate()
{
    //UActorComponent::PostDuplicate();
}

