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

void UMovementComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);

    
    OutProperties.Add(TEXT("Speed"), FString::Printf(TEXT("%f"), Speed));
    OutProperties.Add(TEXT("Velocity"), FString::Printf(TEXT("%s"), *Velocity.ToString()));
    
}

void UMovementComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
    Super::SetProperties(InProperties);

    const FString* TempStr = nullptr;
    TempStr = InProperties.Find(TEXT("Speed"));
    if (TempStr)
    {
        Speed = FString::ToFloat(*TempStr);
    }
    TempStr = InProperties.Find(TEXT("Velocity"));
    if (TempStr)
    {
        Velocity.InitFromString(*TempStr);
    }
    
}

