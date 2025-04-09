#pragma once
#include "Core/HAL/PlatformType.h"
#include <functional> // std::hash 기본 정의 포함

class FString;


class FName
{
    friend struct FNameHelper;

    uint32 DisplayIndex;    // 원본 문자열의 Hash
    uint32 ComparisonIndex; // 비교시 사용되는 Hash

public:
    FName() : DisplayIndex(0), ComparisonIndex(0) {}
    FName(const WIDECHAR* Name);
    FName(const ANSICHAR* Name);
    FName(const FString& Name);

    FString ToString() const;
    uint32 GetDisplayIndex() const { return DisplayIndex; }
    uint32 GetComparisonIndex() const { return ComparisonIndex; }

    bool operator==(const FName& Other) const;
};

extern const FName NAME_None;



namespace std { // std 네임스페이스 안에 정의해야 함

    template<> // FName 타입에 대한 템플릿 특수화
    struct hash<FName>
    {
        // operator()는 const FName&를 인자로 받고 size_t를 반환해야 함
        std::size_t operator()(const FName& Name) const noexcept // noexcept 추가 권장
        {
            // operator==가 ComparisonIndex를 사용하므로 해싱도 ComparisonIndex 기반으로 수행
            // uint32를 size_t로 캐스팅하거나, std::hash<uint32>를 사용하여 추가 해싱 가능
            
            // 가장 간단한 방법: ComparisonIndex를 직접 해시 코드로 사용
            return static_cast<std::size_t>(Name.GetComparisonIndex());

            // 또는 std::hash<uint32> 사용 (결과는 보통 동일하거나 비슷함)
            // return std::hash<uint32>{}(Name.GetComparisonIndex());
        }
    };

} // namespace std