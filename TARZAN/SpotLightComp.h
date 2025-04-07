#pragma once
#include "FireballComp.h"

class USpotLightComponent : public UFireballComponent
{
    DECLARE_CLASS(USpotLightComponent, UFireballComponent)
public:
    USpotLightComponent();
    virtual ~USpotLightComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    float GetInnerSpotAngle() const { return InnerSpotAngle; }
    void SetInnerSpotAngle(float angle) { InnerSpotAngle = angle;}
    float GetOuterSpotAngle() const { return OuterSpotAngle; }
    void SetOuterSpotAngle(float angle) { OuterSpotAngle = angle; }
private:
    float InnerSpotAngle;
    float OuterSpotAngle;
};