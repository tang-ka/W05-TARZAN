#include "SpotLightComp.h"
#include "UObject/ObjectFactory.h"
#include "Define.h"
USpotLightComponent::USpotLightComponent()
{
    SetType(StaticClass()->GetName());
    AABB.max = { 1,1,1 };
    AABB.min = { -1,-1,-1 };
    InnerSpotAngle = 15.0f;
    OuterSpotAngle = 30.0f;
    SetLightType(LightType::SpotLight);
}
USpotLightComponent::USpotLightComponent(const USpotLightComponent& Other)
    : UFireballComponent(Other)
    , InnerSpotAngle(Other.InnerSpotAngle)
    , OuterSpotAngle(Other.OuterSpotAngle)
{
}
USpotLightComponent::~USpotLightComponent()
{
}
void USpotLightComponent::InitializeComponent()
{
    Super::InitializeComponent();
}
void USpotLightComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

void USpotLightComponent::PostDuplicate()
{
    //Super::PostDuplicate();
}

void USpotLightComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);

    OutProperties.Add(TEXT("InnerSpotAngle"), FString::Printf(TEXT("%f"), InnerSpotAngle));
    OutProperties.Add(TEXT("OuterSpotAngle"), FString::Printf(TEXT("%f"), OuterSpotAngle));
}

void USpotLightComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
    Super::SetProperties(InProperties);

    const FString* TempStr = nullptr;
    TempStr = InProperties.Find(TEXT("InnerSpotAngle"));
    if (TempStr)
    {
        InnerSpotAngle = FString::ToFloat(*TempStr);
    }
    TempStr = InProperties.Find(TEXT("OuterSpotAngle"));
    if (TempStr)
    {
        OuterSpotAngle = FString::ToFloat(*TempStr);
    }
}

void USpotLightComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

UObject* USpotLightComponent::Duplicate() const
{
    USpotLightComponent* NewComp = FObjectFactory::ConstructObjectFrom<USpotLightComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}