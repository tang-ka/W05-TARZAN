#include "Components/FireballComp.h"
#include "UObject/ObjectFactory.h"
UFireballComponent::UFireballComponent()
{
    SetType(StaticClass()->GetName());
    AABB.max = { 1,1,1 };
    AABB.min = { -1,-1,-1 };
}

UFireballComponent::UFireballComponent(const UFireballComponent& Other)
    : UPrimitiveComponent(Other)
    , FireballInfo(Other.FireballInfo)
{
}

UFireballComponent::~UFireballComponent()
{
}   
void UFireballComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UFireballComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

void UFireballComponent::PostDuplicate()
{
    //Super::PostDuplicate();
}

void UFireballComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    UPrimitiveComponent::GetProperties(OutProperties);

    OutProperties.Add(TEXT("FireballInfo_Intensity"), FString::Printf(TEXT("%f"), FireballInfo.Intensity));\
    OutProperties.Add(TEXT("FireballInfo_Radius"), FString::Printf(TEXT("%f"), FireballInfo.Radius));
    OutProperties.Add(TEXT("FireballInfo_RadiusFallOff"), FString::Printf(TEXT("%f"), FireballInfo.RadiusFallOff));
    OutProperties.Add(TEXT("FireballInfo_Type"), FString::Printf(TEXT("%d"), static_cast<int>(FireballInfo.Type)));
    
    FVector4 Color = FVector4(FireballInfo.Color.R, FireballInfo.Color.G, FireballInfo.Color.B, FireballInfo.Color.A);
    OutProperties.Add(TEXT("FireballInfo_Color"), FString::Printf(TEXT("%s"), *Color.ToString()));
}

void UFireballComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
    UPrimitiveComponent::SetProperties(InProperties);
    const FString* TempStr = nullptr;
    TempStr = InProperties.Find(TEXT("FireballInfo_Intensity"));
    if (TempStr)
    {
        FireballInfo.Intensity = FString::ToFloat(*TempStr);
    }
    TempStr = InProperties.Find(TEXT("FireballInfo_Radius"));
    if (TempStr)
    {
        FireballInfo.Radius = FString::ToFloat(*TempStr);
    }
    TempStr = InProperties.Find(TEXT("FireballInfo_RadiusFallOff"));
    if (TempStr)
    {
        FireballInfo.RadiusFallOff = FString::ToFloat(*TempStr);
    }
    TempStr = InProperties.Find(TEXT("FireballInfo_Type"));
    if (TempStr)
    {
        FireballInfo.Type = static_cast<LightType>(FString::ToInt(*TempStr));
    }
    TempStr = InProperties.Find(TEXT("FireballInfo_Color"));
    if (TempStr)
    {
        FVector4 Color;
        Color.InitFromString(*TempStr);
        FireballInfo.Color = FLinearColor(Color.x, Color.y, Color.z, Color.a);
    }
    
}

void UFireballComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
    
}

UObject* UFireballComponent::Duplicate() const
{
    UFireballComponent* NewComp = FObjectFactory::ConstructObjectFrom<UFireballComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();

    return NewComp;
}