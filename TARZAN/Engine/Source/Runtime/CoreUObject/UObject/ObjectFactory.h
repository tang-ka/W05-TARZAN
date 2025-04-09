#pragma once
#include "Object.h"
#include "UObjectHash.h"
#include "EngineStatics.h"
#include "UObjectArray.h"
#include "UClass.h"

class FObjectFactory
{
public:
    // UClass*를 기반으로 객체를 생성하는 함수
    static UObject* ConstructObjectFromClass(UClass* ClassToConstruct, UObject* Outer, FName Name = NAME_None)
    {
        if (ClassToConstruct == nullptr)
        {
            UE_LOG(LogLevel::Error, TEXT("ConstructObjectFromClass: ClassToConstruct is null!"));
            return nullptr;
        }

        uint32 id = UEngineStatics::GenUUID();

        FName ObjectName = Name;
        if (ObjectName == NAME_None) // 이름이 제공되지 않으면 자동 생성
        {
            uint32 id = UEngineStatics::GenUUID(); // UUID 생성은 이름 충돌 방지에 좋음
            ObjectName = FName(*FString::Printf(TEXT("%s_%u"), *ClassToConstruct->GetName(), id));
        }

        // UClass에 생성자 함수 포인터가 설정되어 있는지 확인
        if (ClassToConstruct->ObjectConstructor == nullptr)
        {
            // 클래스가 제대로 등록되지 않았거나, 생성자 함수가 연결되지 않음
            UE_LOG(LogLevel::Error, TEXT("ConstructObjectFromClass: Class '%s' does not have a registered constructor function! Ensure IMPLEMENT_CLASS or similar macro is used."), *ClassToConstruct->GetName());
            return nullptr;
        }

        // UClass에 등록된 생성자 함수를 호출하여 객체 생성
        // 이 함수 내부에서 new, 기본 설정, GUObjectArray 추가 등이 이루어짐
        UObject* ConstructedObject = ClassToConstruct->ObjectConstructor(Outer, Name);
        
        /* 생성된 객체 기본 설정 */ \
        ConstructedObject->ClassPrivate = ClassToConstruct;
        ConstructedObject->NamePrivate = ObjectName;
        ConstructedObject->UUID = id;
        // TODO: 리소스는 우선 nullptr 처리 차후 패키징 방법 생각하기
        ConstructedObject->OuterPrivate = Outer;

        GUObjectArray.AddObject(ConstructedObject);

        if (ConstructedObject) {
            UE_LOG(LogLevel::Display, TEXT("Constructed Object: %s (Class: %s)"), *ConstructedObject->GetName(), *ClassToConstruct->GetName());
        } else {
            UE_LOG(LogLevel::Error, TEXT("Failed to construct object of class %s using its constructor function."), *ClassToConstruct->GetName());
        }

        return ConstructedObject;
    }
    
    template <typename T>
        requires std::derived_from<T, UObject>
    static T* ConstructObject(UObject* Outer, FName Name = NAME_None)
    {
        uint32 id = UEngineStatics::GenUUID();
        //FString Name = T::StaticClass()->GetName() + "_" + std::to_string(id);

        FName ObjectName = Name;
        if (ObjectName == NAME_None) // 이름이 제공되지 않으면 자동 생성
        {
            uint32 id = UEngineStatics::GenUUID(); // UUID 생성은 이름 충돌 방지에 좋음
            ObjectName = FName(*FString::Printf(TEXT("%s_%u"), *T::StaticClass()->GetName(), id));
        }

        T* Obj = new T; // TODO: FPlatformMemory::Malloc으로 변경, placement new 사용시 Free방법 생각하기
        Obj->ClassPrivate = T::StaticClass();
        Obj->NamePrivate = ObjectName;
        Obj->UUID = id;

        // TODO: 리소스는 우선 nullptr 처리 차후 패키징 방법 생각하기
        Obj->OuterPrivate = Outer;

        GUObjectArray.AddObject(Obj);

        UE_LOG(LogLevel::Display, "Created New Object : %s", *ObjectName.ToString());
        return Obj;
    }

    template <typename T>
        requires std::derived_from<T, UObject>
    static T* ConstructObjectFrom(const T* Source)
    {
        uint32 id = UEngineStatics::GenUUID();
        FString Name = T::StaticClass()->GetName() + "_Copy_" + std::to_string(id);

        T* Obj = new T(*Source); // 복사 생성자 사용
        Obj->ClassPrivate = T::StaticClass();
        Obj->NamePrivate = Name;
        Obj->UUID = id;

        GUObjectArray.AddObject(Obj);

        UE_LOG(LogLevel::Display, "Cloned Object : %s", *Name);
        return Obj;
    }
};
