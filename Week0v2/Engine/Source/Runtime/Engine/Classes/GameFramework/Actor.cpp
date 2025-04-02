#include "Actor.h"

#include "Engine/World.h"

AActor::AActor(const AActor& Other)
    : UObject(Other),
      RootComponent(nullptr),
      bTickInEditor(Other.bTickInEditor),
      bActorIsBeingDestroyed(Other.bActorIsBeingDestroyed),
      ActorLabel(Other.ActorLabel)
{
    // RootComponent 및 컴포넌트 복사는 DuplicateSubObjects에서 처리
}
void AActor::BeginPlay()
{
    // TODO: 나중에 삭제를 Pending으로 하던가 해서 복사비용 줄이기
    const auto CopyComponents = OwnedComponents;
    for (UActorComponent* Comp : CopyComponents)
    {
        Comp->BeginPlay();
    }
}

void AActor::Tick(float DeltaTime)
{
    if (!RootComponent) return;
    // TODO: 임시로 Actor에서 Tick 돌리기
    // TODO: 나중에 삭제를 Pending으로 하던가 해서 복사비용 줄이기
    const auto CopyComponents = OwnedComponents;
    for (UActorComponent* Comp : CopyComponents)
    {
        if (Comp && Comp->IsComponentTickEnabled())
            Comp->TickComponent(DeltaTime);
    }
}

void AActor::Destroyed()
{
    // Actor가 제거되었을 때 호출하는 EndPlay
    EndPlay(EEndPlayReason::Destroyed);
}

void AActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 본인이 소유하고 있는 모든 컴포넌트의 EndPlay 호출
    for (UActorComponent* Component : GetComponents())
    {
        if (Component->HasBegunPlay())
        {
            Component->EndPlay(EndPlayReason);
        }
    }
    UninitializeComponents();
}


bool AActor::Destroy()
{
    if (!IsActorBeingDestroyed())
    {
        if (UWorld* World = GetWorld())
        {
            World->DestroyActor(this);
            bActorIsBeingDestroyed = true;
        }
    }

    return IsActorBeingDestroyed();
}

void AActor::RemoveOwnedComponent(UActorComponent* Component)
{
    OwnedComponents.Remove(Component);
}

void AActor::InitializeComponents()
{
    TSet<UActorComponent*> Components = GetComponents();
    for (UActorComponent* ActorComp : Components)
    {
        // 먼저 컴포넌트를 등록 처리
        if (!ActorComp->IsRegistered())
        {
            ActorComp->RegisterComponent();
        }

        if (ActorComp->bAutoActive && !ActorComp->IsActive())
        {
            ActorComp->Activate();
        }

        if (!ActorComp->HasBeenInitialized())
        {
            ActorComp->InitializeComponent();
        }
    }
}

void AActor::UninitializeComponents()
{
    TSet<UActorComponent*> Components = GetComponents();
    for (UActorComponent* ActorComp : Components)
    {
        if (ActorComp->HasBeenInitialized())
        {
            ActorComp->UninitializeComponent();
        }
        if (ActorComp->IsRegistered())
        {
            ActorComp->UnregisterComponent();
        }
    }
}

bool AActor::SetRootComponent(USceneComponent* NewRootComponent)
{
    if (NewRootComponent == nullptr || NewRootComponent->GetOwner() == this)
    {
        if (RootComponent != NewRootComponent)
        {
            USceneComponent* OldRootComponent = RootComponent;
            RootComponent = NewRootComponent;

            OldRootComponent->SetupAttachment(RootComponent);
        }
        return true;
    }
    return false;
}

bool AActor::SetActorLocation(const FVector& NewLocation)
{
    if (RootComponent)
    {
        RootComponent->SetLocation(NewLocation);
        return true;
    }
    return false;
}

bool AActor::SetActorRotation(const FVector& NewRotation)
{
    if (RootComponent)
    {
        RootComponent->SetRotation(NewRotation);
        return true;
    }
    return false;
}

bool AActor::SetActorScale(const FVector& NewScale)
{
    if (RootComponent)
    {
        RootComponent->SetScale(NewScale);
        return true;
    }
    return false;
}
// AActor.cpp
void AActor::AddComponent(UActorComponent* Component)
{
    OwnedComponents.Add(Component);
    Component->Owner = this;

    if (USceneComponent* SceneComp = Cast<USceneComponent>(Component))
    {
        if (RootComponent == nullptr)
        {
            RootComponent = SceneComp;
        }
        else
        {
            SceneComp->SetupAttachment(RootComponent);
        }
    }

    Component->InitializeComponent();
}


UObject* AActor::Duplicate() const
{
    AActor* ClonedActor = FObjectFactory::ConstructObjectFrom<AActor>(this);
    ClonedActor->DuplicateSubObjects();
    ClonedActor->PostDuplicate();
    return ClonedActor;
}
void AActor::DuplicateSubObjects()
{
    UObject::DuplicateSubObjects(); // 부모의 하위 오브젝트 복사 (필요 시)
    TMap<const USceneComponent*, USceneComponent*> SceneCloneMap;

    for (UActorComponent* Component : OwnedComponents)
    {
        UActorComponent* ClonedComponent = static_cast<UActorComponent*>(Component->Duplicate());
        ClonedComponent->Owner = this;
        AddComponent(ClonedComponent);

        if (const USceneComponent* OldScene = Cast<USceneComponent>(Component))
        {
            if (USceneComponent* NewScene = Cast<USceneComponent>(ClonedComponent))
            {
                SceneCloneMap.Add(OldScene, NewScene);
            }
        }
    }

    for (const auto& Pair : SceneCloneMap)
    {
        const USceneComponent* Old = Pair.Key;
        USceneComponent* New = Pair.Value;
        if (const USceneComponent* OldParent = Old->GetAttachParent())
        {
            if (USceneComponent** Found = SceneCloneMap.Find(OldParent))
            {
                USceneComponent* NewParent = *Found;
                New->SetupAttachment(NewParent);
            }
        }
    }

    if (RootComponent)
    {
        if (USceneComponent** Found = SceneCloneMap.Find(RootComponent))
        {
            SetRootComponent(*Found);
        }
    }
}
void AActor::PostDuplicate()
{
    // Override in subclasses if needed
}