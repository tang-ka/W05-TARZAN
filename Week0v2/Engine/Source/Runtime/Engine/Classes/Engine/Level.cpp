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
    CloneLevel->DuplicateSubObjects(this);
    CloneLevel->PostDuplicate();
    return CloneLevel;
}

void ULevel::DuplicateSubObjects(const UObject* SourceObj)
{
    UObject::DuplicateSubObjects(SourceObj);
    for (AActor* Actor : Actors)
    {
        Actor = static_cast<AActor*>(Actor->Duplicate()); 
    }
}

void ULevel::PostDuplicate()
{
    UObject::PostDuplicate();
}
