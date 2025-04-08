#include "FireballComp.h"
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