#include "FireballComp.h"

UFireballComponent::UFireballComponent()
{
    SetType(StaticClass()->GetName());
    AABB.max = { 1,1,1 };
    AABB.min = { -1,-1,-1 };
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