#pragma once
#include "Components/PrimitiveComponent.h"
#include "Color.h"
#include "Define.h"
class UFireballComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UFireballComponent, UPrimitiveComponent)

public:
    UFireballComponent();
    virtual ~UFireballComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;

    FFireballInfo GetFireballInfo() const { return FireballInfo; }
private:
    FFireballInfo FireballInfo;
};

