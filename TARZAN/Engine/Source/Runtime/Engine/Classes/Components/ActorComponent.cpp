#include "ActorComponent.h"

#include "GameFramework/Actor.h"

UActorComponent::UActorComponent(const UActorComponent& Other)
    : UObject(Other),
      bCanEverTick(Other.bCanEverTick),
      bRegistered(Other.bRegistered),
      bWantsInitializeComponent(Other.bWantsInitializeComponent),
      bHasBeenInitialized(Other.bHasBeenInitialized),
      bHasBegunPlay(Other.bHasBegunPlay),
      bIsBeingDestroyed(Other.bIsBeingDestroyed),
      bIsActive(Other.bIsActive),
      bTickEnabled(Other.bTickEnabled),
      bAutoActive(Other.bAutoActive)
{
    // Owner는 복제 시점에 AActor가 직접 지정
}

void UActorComponent::InitializeComponent()
{
    assert(!bHasBeenInitialized);

    bHasBeenInitialized = true;
}

void UActorComponent::UninitializeComponent()
{
    assert(bHasBeenInitialized);

    bHasBeenInitialized = false;
}

void UActorComponent::BeginPlay()
{
    bHasBegunPlay = true;
}

void UActorComponent::TickComponent(float DeltaTime)
{
}

void UActorComponent::OnComponentDestroyed()
{
}

void UActorComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    TMap<FString, FString>& Properties = OutProperties;

    Properties.Add(TEXT("ComponentName"), *GetName());
    Properties.Add(TEXT("ComponentClass"), *GetClass()->GetName());
    //Properties.Add(TEXT("ComponentUUID"), std::to_string(GetUUID()));
    //Properties.Add(TEXT("ComponentOwnerUUID"), FString::Printf(TEXT("%u"), GetOwner()->GetUUID()))
    Properties.Add(TEXT("ComponentOwner"), *GetOwner()->GetName());
    Properties.Add(TEXT("ComponentOwnerClass"), *GetOwner()->GetClass()->GetName());

    
    Properties.Add(TEXT("bCanEverTick"), bCanEverTick ? TEXT("true") : TEXT("false"));
    //Properties.Add(TEXT("bRegistered"), bRegistered ? TEXT("true") : TEXT("false"));
    Properties.Add(TEXT("bWantsInitializeComponent"), bWantsInitializeComponent ? TEXT("true") : TEXT("false"));
    Properties.Add(TEXT("bIsActive"), bIsActive ? TEXT("true") : TEXT("false"));
    Properties.Add(TEXT("bTickEnabled"), bTickEnabled ? TEXT("true") : TEXT("false"));
    Properties.Add(TEXT("bAutoActive"), bAutoActive ? TEXT("true") : TEXT("false"));
    

    
    return; // 기본적으로는 비어있거나 최소한의 정보만 포함
}

void UActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    assert(bHasBegunPlay);

    bHasBegunPlay = false;
}

void UActorComponent::DestroyComponent()
{
    if (bIsBeingDestroyed)
    {
        return;
    }

    bIsBeingDestroyed = true;

    // Owner에서 Component 제거하기
    if (AActor* MyOwner = GetOwner())
    {
        MyOwner->RemoveOwnedComponent(this);
        if (MyOwner->GetRootComponent() == this)
        {
            MyOwner->SetRootComponent(nullptr);
        }
    }

    if (bHasBegunPlay)
    {
        EndPlay(EEndPlayReason::Destroyed);
    }

    if (bHasBeenInitialized)
    {
        UninitializeComponent();
    }

    OnComponentDestroyed();

    // 나중에 ProcessPendingDestroyObjects에서 실제로 제거됨
    GUObjectArray.MarkRemoveObject(this);
}

void UActorComponent::Activate()
{
    // TODO: Tick 다시 재생
    bIsActive = true;
}

void UActorComponent::Deactivate()
{
    // TODO: Tick 멈추기
    bIsActive = false;
}

void UActorComponent::OnRegister()
{
    // Hook: Called by RegisterComponent()
    if (bAutoActive)
    {
        Activate();
    }

    if (bWantsInitializeComponent && !bHasBeenInitialized)
    {
        InitializeComponent();
    }
}

void UActorComponent::OnUnregister()
{
    // Hook: Called by UnregisterComponent()
    Deactivate();
}

void UActorComponent::RegisterComponent()
{
    if (bRegistered)
        return;

    bRegistered = true;
    OnRegister();
}

void UActorComponent::UnregisterComponent()
{
    if (!bRegistered)
        return;

    OnUnregister();
    bRegistered = false;
}
UObject* UActorComponent::Duplicate() const
{
    UActorComponent* NewComp = FObjectFactory::ConstructObjectFrom<UActorComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}
void UActorComponent::DuplicateSubObjects(const UObject* Source)
{
    UObject::DuplicateSubObjects(Source);
}
void UActorComponent::PostDuplicate()
{
    
}