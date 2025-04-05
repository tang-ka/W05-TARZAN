#include "ProjectileMovementComponent.h"

#include "GameFramework/Actor.h"
#include "UObject/ObjectFactory.h"

UProjectileMovementComponent::UProjectileMovementComponent()
{

}

UProjectileMovementComponent::UProjectileMovementComponent(const UProjectileMovementComponent& Other)
    : UMovementComponent(Other)
{
}

UProjectileMovementComponent* UProjectileMovementComponent::Duplicate() const
{
    UProjectileMovementComponent* NewComp = FObjectFactory::ConstructObjectFrom<UProjectileMovementComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    
    return NewComp;
}

void UProjectileMovementComponent::DuplicateSubObjects(const UObject* Source)
{
    UMovementComponent::DuplicateSubObjects(Source);
}

void UProjectileMovementComponent::PostDuplicate()
{
}



void UProjectileMovementComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
    
    AActor* Owner = GetOwner(); // 이 컴포넌트를 소유한 액터 가져오기
    if (Owner) // Owner가 유효한지 항상 확인
    {
        FVector DeltaLocation = Velocity * DeltaTime; // 속도에 따라 이동할 거리 계산
        Owner->AddActorLocalOffset(DeltaLocation); // 액터를 이동
    }
}
