#include "Components/UHeightFogComponent.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Launch/EditorEngine.h"
#include "Editor/LevelEditor/SLevelEditor.h"
#include "PropertyEditor/ShowFlags.h"
#include "UObject/ObjectFactory.h"

extern UEditorEngine* GEngine;

UHeightFogComponent::UHeightFogComponent()
{ 
    FogDensity = 1.00f;
    FogHeightFalloff = 0.1f;
    StartDistance = 5.0f;
    FogCutoffDistance = 800.0f;
    FogMaxOpacity = 1.0f;
    FogInscatteringColor = FLinearColor(0.6f, 0.1f, 0.1f, 1.0f); // 어두운 빨강

    std::shared_ptr<FEditorViewportClient> ActiveViewport = GEngine->GetLevelEditor()->GetActiveViewportClient();
    DisableFog = (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Fog)) ? 0 : 1;
}

UHeightFogComponent::UHeightFogComponent(const UHeightFogComponent& Other)
    : USceneComponent(Other)
    , FogDensity(Other.FogDensity)
    , FogHeightFalloff(Other.FogHeightFalloff)
    , StartDistance(Other.StartDistance)
    , FogCutoffDistance(Other.FogCutoffDistance)
    , FogMaxOpacity(Other.FogMaxOpacity)
    , FogInscatteringColor(Other.FogInscatteringColor)
{
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

UObject* UHeightFogComponent::Duplicate() const
{
    UHeightFogComponent* NewComp = FObjectFactory::ConstructObjectFrom<UHeightFogComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void UHeightFogComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void UHeightFogComponent::PostDuplicate()
{
    //Super::PostDuplicate();
}

void UHeightFogComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);
    OutProperties.Add(TEXT("FogDensity"), FString::Printf(TEXT("%f"), FogDensity));
    OutProperties.Add(TEXT("FogHeightFalloff"), FString::Printf(TEXT("%f"), FogHeightFalloff));
    OutProperties.Add(TEXT("StartDistance"), FString::Printf(TEXT("%f"), StartDistance));
    OutProperties.Add(TEXT("FogCutoffDistance"), FString::Printf(TEXT("%f"), FogCutoffDistance));
    OutProperties.Add(TEXT("FogMaxOpacity"), FString::Printf(TEXT("%f"), FogMaxOpacity));
    FVector4 Color = FVector4(FogInscatteringColor.R, FogInscatteringColor.G, FogInscatteringColor.B, FogInscatteringColor.A);
    
    OutProperties.Add(TEXT("FogInscatteringColor"), FString::Printf(TEXT("%s"), *Color.ToString()));
    OutProperties.Add(TEXT("DisableFog"), FString::Printf(TEXT("%f"), DisableFog));
    
}

void UHeightFogComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
    Super::SetProperties(InProperties);
    const FString* TempStr = nullptr;
    TempStr = InProperties.Find(TEXT("FogDensity"));
    if (TempStr)
    {
        FogDensity = FString::ToFloat(*TempStr);
    }
    TempStr = InProperties.Find(TEXT("FogHeightFalloff"));
    if (TempStr)
    {
        FogHeightFalloff = FString::ToFloat(*TempStr);
    }
    TempStr = InProperties.Find(TEXT("StartDistance"));
    if (TempStr)
    {
        StartDistance = FString::ToFloat(*TempStr);
    }
    TempStr = InProperties.Find(TEXT("FogCutoffDistance"));
    if (TempStr)
    {
        FogCutoffDistance = FString::ToFloat(*TempStr);
    }
    TempStr = InProperties.Find(TEXT("FogMaxOpacity"));
    if (TempStr)
    {
        FogMaxOpacity = FString::ToFloat(*TempStr);
    }
    TempStr = InProperties.Find(TEXT("FogInscatteringColor"));
    if (TempStr)
    {
        FVector4 Color;
        Color.InitFromString(*TempStr);
        FogInscatteringColor = FLinearColor(Color.x, Color.y, Color.z, Color.a);
    }
    TempStr = InProperties.Find(TEXT("DisableFog"));
    if (TempStr)
    {
        DisableFog = FString::ToFloat(*TempStr);
    }
    // FogData.FogDensity = FogDensity;
}

