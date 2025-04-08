#pragma once
#include "Components/SceneComponent.h"
class UHeightFogComponent :
    public USceneComponent
{
    DECLARE_CLASS(UHeightFogComponent, USceneComponent)

public:
    UHeightFogComponent();
    UHeightFogComponent(const UHeightFogComponent& Other);
    virtual ~UHeightFogComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;

    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;
    // Setters
    void SetFogDensity(float density) { FogDensity = density; }
    void SetFogHeightFalloff(float falloff) { FogHeightFalloff = falloff; }
    void SetStartDistance(float distance) { StartDistance = distance; }
    void SetFogCutoffDistance(float distance) { FogCutoffDistance = distance; }
    void SetFogMaxOpacity(float opacity) { FogMaxOpacity = opacity; }
    void SetColor(const FLinearColor& color) { FogInscatteringColor = color; }
    // Getters
    float GetFogDensity() const { return FogDensity; }
    float GetFogHeightFalloff() const { return FogHeightFalloff; }
    float GetStartDistance() const { return StartDistance; }
    float GetFogCutoffDistance() const { return FogCutoffDistance; }
    float GetFogMaxOpacity() const { return FogMaxOpacity; }
    FLinearColor GetColor() const { return FogInscatteringColor; }


private:
    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;
    float FogCutoffDistance;
    float FogMaxOpacity;

    FLinearColor FogInscatteringColor;
};

