#pragma once
#include "PrimitiveComponent.h"
#include "Define.h"
class UBillboardComponent;

class ULightComponentBase : public USceneComponent
{
    DECLARE_CLASS(ULightComponentBase, USceneComponent)

public:
    ULightComponentBase();
    virtual ~ULightComponentBase() override;

    virtual void TickComponent(float DeltaTime) override;
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);
    void InitializeLight();
    void SetColor(FLinearColor newColor);
    FLinearColor GetColor() const;
    float GetRadius() const;
    void SetRadius(float r);

private:
    FLinearColor color;
    float radius;
    FBoundingBox AABB;
    UBillboardComponent* texture2D;
public:
    FBoundingBox GetBoundingBox() const {return AABB;}
    float GetRadius() {return radius;}
    FLinearColor GetColor() {return color;}
    UBillboardComponent* GetTexture2D() const {return texture2D;}
};
