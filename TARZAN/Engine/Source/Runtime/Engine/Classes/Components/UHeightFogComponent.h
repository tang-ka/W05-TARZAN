#pragma once
#include "Components/SceneComponent.h"
#include <functional>

class UHeightFogComponent : public USceneComponent
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

    void GetProperties(TMap<FString, FString>& OutProperties) const override;
    
    void SetProperties(const TMap<FString, FString>& InProperties) override;

    
    // Setters
    void SetFogDensity(float density) { FogDensity = density; TriggerFogChanged(); }
    void SetFogHeightFalloff(float falloff) { FogHeightFalloff = falloff; TriggerFogChanged(); }
    void SetStartDistance(float distance) { StartDistance = distance; TriggerFogChanged(); }
    void SetFogCutoffDistance(float distance) { FogCutoffDistance = distance; TriggerFogChanged(); }
    void SetFogMaxOpacity(float opacity) { FogMaxOpacity = opacity; TriggerFogChanged(); }
    void SetColor(const FLinearColor& color) { FogInscatteringColor = color; TriggerFogChanged(); }
    void SetDisableFog(bool bDisable) { DisableFog = bDisable ? 1.0f : 0.0f; TriggerFogChanged(); }

    // Getters
    float GetFogDensity() const { return FogDensity; }
    float GetFogHeightFalloff() const { return FogHeightFalloff; }
    float GetStartDistance() const { return StartDistance; }
    float GetFogCutoffDistance() const { return FogCutoffDistance; }
    float GetFogMaxOpacity() const { return FogMaxOpacity; }
    FLinearColor GetColor() const { return FogInscatteringColor; }
    float GetDisableFog() const { return DisableFog; }

    // 이벤트 콜백. 값 변경시 호출됨.
    std::function<void()> OnFogChanged;

private:
    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;
    float FogCutoffDistance;
    float FogMaxOpacity;
    FLinearColor FogInscatteringColor;
    float DisableFog;

    // 내부에서 값 변경 시 이벤트 트리거
    void TriggerFogChanged()
    {
        if (OnFogChanged)
            OnFogChanged();
    }
};
