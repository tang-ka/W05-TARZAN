#include "Object.h"

#include "EditorEngine.h"
#include "UClass.h"
#include "UObjectHash.h"


UClass* UObject::StaticClass()
{
    static UClass ClassInfo{TEXT("UObject"), sizeof(UObject), alignof(UObject), nullptr};
    return &ClassInfo;
}

UObject::UObject()
    : UUID(0)
    // TODO: Object를 생성할 때 직접 설정하기
    , InternalIndex(std::numeric_limits<uint32>::max())
    , NamePrivate("None")
{
}

UWorld* UObject::GetWorld()
{
    return GEngine->GetWorld().get();
}

bool UObject::IsA(const UClass* SomeBase) const
{
    const UClass* ThisClass = GetClass();
    return ThisClass->IsChildOf(SomeBase);
}

void UObject::DuplicateSubObjects(FDuplicationMap& DupMap)
{

}

UObject* UObject::Duplicate()
{
    FDuplicationMap DupMap;
    return Duplicate(DupMap);
}

UObject* UObject::Duplicate(FDuplicationMap& DupMap)
{
    if (DupMap.find(this) != DupMap.end())
    {
        return DupMap[this];
    }

    // 새 객체 생성 (얕은 복사 수행)
    UObject* NewObject = new UObject(*this);

    // 복제 맵에 등록
    DupMap[this] = NewObject;

    // 서브 오브젝트는 깊은 복사로 처리
    NewObject->DuplicateSubObjects(DupMap);
    
    return NewObject;
}

