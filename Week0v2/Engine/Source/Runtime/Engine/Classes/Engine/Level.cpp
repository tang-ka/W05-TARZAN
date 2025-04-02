#include "Level.h"
#include "GameFramework/Actor.h"

ULevel::ULevel()
{
}

ULevel::~ULevel()
{
}

void ULevel::DuplicateSubObjects(FDuplicationMap& DupMap)
{
    for (AActor* Actor : Actors)
    {
        if (Actor)
        {
            Actor = Cast<AActor>(Actor->Duplicate(DupMap));
            Actor->DuplicateSubObjects(DupMap);
        }
    }
}
