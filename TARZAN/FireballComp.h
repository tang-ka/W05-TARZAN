#pragma once
#include "Components/PrimitiveComponent.h"
#include "Color.h"
#include "Define.h"
class UFireballComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UFireballComponent, UPrimitiveComponent)

public:
    UFireballComponent();
    UFireballComponent(const UFireballComponent& Other);
    virtual ~UFireballComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;


    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;


    void SetColor(const FLinearColor & Color) { FireballInfo.Color = Color; }
    void SetRadius(float Radius) { FireballInfo.Radius = Radius; }
    void SetIntensity(float Intensity) { FireballInfo.Intensity = Intensity; }
    float GetIntensity() { return FireballInfo.Intensity; }
    float GetRadius() { return FireballInfo.Radius; }
    FLinearColor GetColor() { return FireballInfo.Color; }
    int GetLightType() { return FireballInfo.Type; }
    void SetLightType(LightType LightType) { FireballInfo.Type = LightType; }
    
    FFireballInfo GetFireballInfo() const { return FireballInfo; }
private:
    FFireballInfo FireballInfo;
};

