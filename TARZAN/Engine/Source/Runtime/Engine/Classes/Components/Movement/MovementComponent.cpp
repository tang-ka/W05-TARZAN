#include "MovementComponent.h"

#include "UObject/ObjectFactory.h"

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

UMovementComponent::UMovementComponent()
{
}

UMovementComponent::UMovementComponent(const UMovementComponent& Other)
    : UActorComponent(Other)
{
}
