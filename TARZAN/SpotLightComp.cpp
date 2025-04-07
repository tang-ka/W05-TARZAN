#include "SpotLightComp.h"
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