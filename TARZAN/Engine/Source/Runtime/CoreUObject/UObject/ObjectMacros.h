// ReSharper disable CppClangTidyBugproneMacroParentheses
#pragma once
#include "UClass.h"

// name을 문자열화 해주는 매크로
#define INLINE_STRINGIFY(name) #name


// RTTI를 위한 클래스 매크로
#define DECLARE_CLASS(TClass, TSuperClass) \
private: \
    /**TClass(const TClass&) = delete;*/\
    TClass& operator=(const TClass&) = delete; \
    TClass(TClass&&) = delete; \
    TClass& operator=(TClass&&) = delete; \
public: \
    using Super = TSuperClass; \
    using ThisClass = TClass; \
    /* 1. 정적 생성자 헬퍼 함수 정의 */ \
    static UObject* Construct_##TClass(UObject* Outer, FName Name) \
    { \
        /* 실제 객체 생성 (new 사용) */ \
        TClass* Obj = new TClass(); \
        return Obj; \
    } \
\
    static UClass* StaticClass() { \
        static UClass ClassInfo{ TEXT(#TClass), static_cast<uint32>(sizeof(TClass)), static_cast<uint32>(alignof(TClass)), TSuperClass::StaticClass() }; \
                    /* !!! 중요: 생성자 함수 포인터 연결 !!! */ \
        ClassInfo.ObjectConstructor = &Construct_##TClass; \
        return &ClassInfo; \
    } \
struct FAutoRegister##TClass { \
    FAutoRegister##TClass() { \
    GetGlobalClassRegistry().Add(FName(TEXT(#TClass)), &TClass::StaticClass); \
        } \
    }; \
friend struct FAutoRegister##TClass;\
/* 자동 등록용 구조체 정의 (private 내부) */ \
private: \
inline static FAutoRegister##TClass AutoRegister##TClass##Instance; \
public: \

// #define PROPERTY(Type, VarName, DefaultValue) \
// private: \
//     Type VarName DefaultValue; \
// public: \
//     Type Get##VarName() const { return VarName; } \
//     void Set##VarName(const Type& New##VarName) { VarName = New##VarName; }

// Getter & Setter 생성
#define PROPERTY(type, name) \
    void Set##name(const type& value) { name = value; } \
    type Get##name() const { return name; }