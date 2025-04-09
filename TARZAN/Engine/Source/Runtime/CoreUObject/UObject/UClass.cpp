#include "UClass.h"
#include <cassert>


UClass::UClass(const char* InClassName, uint32 InClassSize, uint32 InAlignment, UClass* InSuperClass)
    : ClassSize(InClassSize)
    , ClassAlignment(InAlignment)
    , SuperClass(InSuperClass)
{
    NamePrivate = InClassName;
}

bool UClass::IsChildOf(const UClass* SomeBase) const
{
    assert(this);
    if (!SomeBase) return false;

    // Super의 Super를 반복하면서 
    for (const UClass* TempClass = this; TempClass; TempClass=TempClass->GetSuperClass())
    {
        if (TempClass == SomeBase)
        {
            return true;
        }
    }
    return false;
}

UObject* UClass::CreateDefaultObject()
{
    if (!ClassDefaultObject)
    {
        // TODO: CDO 객체 만들기
        ClassDefaultObject = nullptr;
    }

    return ClassDefaultObject;
}

TMap<FName, FGetClassFunction>& GetGlobalClassRegistry()
{
    // 함수 내 지역 정적 변수: 처음 호출될 때 단 한 번 초기화됨
    static TMap<FName, FGetClassFunction> ActualRegistryInstance;
    return ActualRegistryInstance;
}

UClass* FindClassByName(FName ClassName)
{
    {
        FGetClassFunction* FoundFunc = GetGlobalClassRegistry().Find(ClassName);
        if (FoundFunc)
        {
            return (*FoundFunc)(); // 등록된 StaticClass 함수 호출
        }
        return nullptr; // 등록되지 않은 클래스
    }
}

UClass* FindClassByName(const FString& ClassName)
{
    return FindClassByName(FName(*ClassName));
}


// 전역 레지스트리 정의
//TMap<FName, FGetClassFunction> GClassRegistry;