#pragma once
#include "Components/PrimitiveComponent.h"
#include "Color.h"
class UFireballComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UFireballComponent, UPrimitiveComponent)

public:
    UFireballComponent();
    virtual ~UFireballComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;

    float Intensity;
    float Radius;
    float RadiusFallOff;
    FLinearColor Color;
};

