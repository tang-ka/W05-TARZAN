#pragma once

#include "Define.h"
#include "NameTypes.h"
#include "Container/Map.h"
#include "Container/String.h"


class UEditorEngine;
extern UEditorEngine* GEngine;

class UClass;
class UWorld;


class UObject
{
private:
    UObject& operator=(const UObject&) = delete;
    UObject(UObject&&) = delete;
    UObject& operator=(UObject&&) = delete;

public:
    using Super = UObject;
    using ThisClass = UObject;
    UObject(const UObject& Other)
        : ClassPrivate(Other.ClassPrivate)
        , NamePrivate(Other.NamePrivate)
        , UUID(Other.UUID)
        , InternalIndex(Other.InternalIndex)
    {
    }
    static UClass* StaticClass();

    /* 1. 정적 생성자 헬퍼 함수 정의 */ \
    static UObject* Construct_UObject(UObject* Outer, FName Name) \
    { \
    /* 실제 객체 생성 (new 사용) */ \
        UObject* Obj = new UObject(); \
    /* 생성된 객체 기본 설정 */ \
        Obj->ClassPrivate = UObject::StaticClass(); \
    return Obj; \
    } \
    
    struct FAutoRegisterUObject { 
    FAutoRegisterUObject();
    }; 
    static FAutoRegisterUObject AutoRegisterUObjectInstance; 

    virtual UObject* Duplicate() const
    {
        UObject* NewObject = new UObject();
        NewObject->DuplicateSubObjects(this);       // 깊은 복사 수행
        return NewObject;
    }

    virtual void DuplicateSubObjects(const UObject* Source){}; // 하위 클래스에서 override
    virtual void PostDuplicate(){};
private:
    friend class FObjectFactory;
    friend class FSceneMgr;
    friend class UClass;

    uint32 UUID;
    uint32 InternalIndex; // Index of GUObjectArray

    FName NamePrivate;
    UClass* ClassPrivate = nullptr;
    UObject* OuterPrivate = nullptr; // Outer 객체의 포인터

public:
    UObject();
    virtual ~UObject() = default;

    UWorld* GetWorld();
    ;

    UEditorEngine* GetEngine()
    {
        return GEngine;
    }

    FName GetFName() const { return NamePrivate; }
    FString GetName() const { return NamePrivate.ToString(); }

    uint32 GetUUID() const { return UUID; }
    uint32 GetInternalIndex() const { return InternalIndex; }

    UClass* GetClass() const { return ClassPrivate; }
    UObject* GetOuter() const { return OuterPrivate; }
    


    /** this가 SomeBase인지, SomeBase의 자식 클래스인지 확인합니다. */
    bool IsA(const UClass* SomeBase) const;

    template <typename T>
        requires std::derived_from<T, UObject>
    bool IsA() const
    {
        return IsA(T::StaticClass());
    }

public:
    void* operator new(size_t size)
    {
        UE_LOG(LogLevel::Display, "UObject Created : %d", size);

        void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(size);
        UE_LOG(
            LogLevel::Display,
            "TotalAllocationBytes : %d, TotalAllocationCount : %d",
            FPlatformMemory::GetAllocationBytes<EAT_Object>(),
            FPlatformMemory::GetAllocationCount<EAT_Object>()
        );
        return RawMemory;
    }

    void operator delete(void* ptr, size_t size)
    {
        UE_LOG(LogLevel::Display, "UObject Deleted : %d", size);
        FPlatformMemory::Free<EAT_Object>(ptr, size);
    }

    FVector4 EncodeUUID() const {
        FVector4 result;

        result.x = UUID % 0xFF;
        result.y = UUID >> 8 & 0xFF;
        result.z = UUID >> 16 & 0xFF;
        result.a = UUID >> 24 & 0xFF;

        return result;
    }
private:
};
