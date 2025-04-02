#include "Level.h"
#include "GameFramework/Actor.h"

ULevel::ULevel()
{
}

ULevel::~ULevel()
{
}

UObject* ULevel::Duplicate() const
{
    ULevel* CloneLevel = FObjectFactory::ConstructObjectFrom<ULevel>(this);
    CloneLevel->DuplicateSubObjects();
    CloneLevel->PostDuplicate();
    return CloneLevel;
}

void ULevel::DuplicateSubObjects()
{
    UObject::DuplicateSubObjects();
    for (AActor* Actor : Actors)
    {
        AActor* DupActor = static_cast<AActor*>(Actor->Duplicate()); 
    }
}

void ULevel::PostDuplicate()
{
    UObject::PostDuplicate();
}
