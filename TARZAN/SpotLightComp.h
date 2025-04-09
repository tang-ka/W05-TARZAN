#pragma once
#include "Components/FireballComp.h"

class USpotLightComponent : public UFireballComponent
{
    DECLARE_CLASS(USpotLightComponent, UFireballComponent)
public:
    USpotLightComponent();
    USpotLightComponent(const USpotLightComponent& Other);
    virtual ~USpotLightComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    float GetInnerSpotAngle() const { return InnerSpotAngle; }
    void SetInnerSpotAngle(float angle) { InnerSpotAngle = angle;}
    float GetOuterSpotAngle() const { return OuterSpotAngle; }
    void SetOuterSpotAngle(float angle) { OuterSpotAngle = angle; }

    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;
private:
    float InnerSpotAngle;
    float OuterSpotAngle;
};