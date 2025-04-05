#include "RotatingMovementComponent.h"

#include "GameFramework/Actor.h"
#include "UObject/ObjectFactory.h"

URotatingMovementComponent::URotatingMovementComponent()
{
    FVector RotationRate = FVector(0.f, 0.f, 1.f); // 회전 속도 설정
    float RotationSpeed = 1.f; // 회전 속도 설정
}

URotatingMovementComponent::URotatingMovementComponent(const URotatingMovementComponent& Other)
    : UMovementComponent(Other),
      RotationRate(Other.RotationRate),
      RotationSpeed(Other.RotationSpeed)
{
    
}

URotatingMovementComponent* URotatingMovementComponent::Duplicate() const
{
    URotatingMovementComponent* NewComp = FObjectFactory::ConstructObjectFrom<URotatingMovementComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    
    return NewComp;
}

void URotatingMovementComponent::DuplicateSubObjects(const UObject* Source)
{
    UMovementComponent::DuplicateSubObjects(Source);
}

void URotatingMovementComponent::PostDuplicate()
{
}

void URotatingMovementComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
    
    AActor* Owner = GetOwner(); // 이 컴포넌트를 소유한 액터 가져오기
    if (Owner) // Owner가 유효한지 항상 확인
    {
        FVector DeltaRotation = RotationRate * DeltaTime * RotationSpeed; // 속도에 따라 이동할 거리 계산
        Owner->AddActorLocalRotation(DeltaRotation); // 액터를 이동
    }
}
