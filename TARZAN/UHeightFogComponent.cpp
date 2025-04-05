#include "UHeightFogComponent.h"

UHeightFogComponent::UHeightFogComponent()
{ 
    FogDensity = 0.02f;
    FogHeightFalloff = 0.1f;
    StartDistance = 100.0f;
    FogCutoffDistance = 800.0f;
    FogMaxOpacity = 0.6f;
    FogInscatteringColor = FLinearColor(0.6f, 0.1f, 0.1f, 1.0f); // 어두운 빨강
}
UHeightFogComponent::~UHeightFogComponent()
{
}
void UHeightFogComponent::InitializeComponent()
{
    Super::InitializeComponent();
}
void UHeightFogComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}
