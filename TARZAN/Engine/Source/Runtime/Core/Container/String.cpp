#include "String.h"
#include <algorithm>
#include <cctype>

#include "Math/MathUtility.h"


#if USE_WIDECHAR
std::wstring FString::ConvertToWideChar(const ANSICHAR* NarrowStr)
{
	const int Size = MultiByteToWideChar(CP_UTF8, 0, NarrowStr, -1, nullptr, 0);
	std::wstring Str;
	Str.resize(Size - 1);
	MultiByteToWideChar(CP_UTF8, 0, NarrowStr, -1, Str.data(), Size);
	return Str;
}
#endif


FString FString::SanitizeFloat(float InFloat)
{
#if USE_WIDECHAR
    return FString{std::to_wstring(InFloat)};
#else
    return FString{std::to_string(InFloat)};
#endif
}

float FString::ToFloat(const FString& InString)
{
	return std::stof(*InString);
}

int FString::ToInt(const FString& InString)
{
    return std::stoi(*InString);
}

FString FString::RightChop(int32 Count) const
{
    const int32 MyLen = Len(); // 현재 문자열 길이

    // Count가 0 이하이면 원본 문자열의 복사본을 반환
    if (Count <= 0)
    {
        return *this; // 복사본 반환
    }

    // Count가 문자열 길이 이상이면 빈 문자열 반환
    if (Count >= MyLen)
    {
        return FString(); // 기본 생성된 빈 FString 반환
    }

    // std::basic_string::substr(pos)는 위치 pos부터 끝까지의 부분 문자열을 반환합니다.
    // Count는 제거할 문자의 개수이므로, 부분 문자열은 Count 인덱스부터 시작합니다.
    // static_cast<size_t>는 substr이 size_t를 인자로 받기 때문에 필요합니다.
    BaseStringType Substring = PrivateString.substr(static_cast<size_t>(Count));

    // 추출된 부분 문자열로 새로운 FString 객체를 생성하여 반환
    // std::move를 사용하면 불필요한 복사를 피할 수 있습니다 (C++11 이상).
    return FString(std::move(Substring));
}

void FString::Empty()
{
    PrivateString.clear();
}

bool FString::Equals(const FString& Other, ESearchCase::Type SearchCase) const
{
    const int32 Num = Len();
    const int32 OtherNum = Other.Len();

    if (Num != OtherNum)
    {
        // Handle special case where FString() == FString("")
        return Num + OtherNum == 1;
    }
    else if (Num > 1)
    {
        if (SearchCase == ESearchCase::CaseSensitive)
        {
        	return TCString<ElementType>::Strcmp(**this, *Other) == 0;
        }
        else
        {
        	return std::ranges::equal(
		        PrivateString, Other.PrivateString, [](char a, char b)
	        {
		        return std::tolower(a) == std::tolower(b);
	        });
        }
    }

    return true;
}

bool FString::Contains(const FString& SubStr, ESearchCase::Type SearchCase, ESearchDir::Type SearchDir) const
{
    return Find(SubStr, SearchCase, SearchDir, 0) != INDEX_NONE;
}

int32 FString::Find(
    const FString& SubStr, ESearchCase::Type SearchCase, ESearchDir::Type SearchDir, int32 StartPosition
) const
{
    if (SubStr.IsEmpty() || IsEmpty())
    {
        return INDEX_NONE;
    }

    const ElementType* StrPtr = **this;
    const ElementType* SubStrPtr = *SubStr;
    const int32 StrLen = Len();
    const int32 SubStrLen = SubStr.Len();

    auto CompareFunc = [SearchCase](ElementType A, ElementType B) -> bool {
        return (SearchCase == ESearchCase::IgnoreCase) ? 
            tolower(A) == tolower(B) : A == B;
    };

    auto FindSubString = [&](int32 Start, int32 End, int32 Step) -> int32 {
        for (int32 i = Start; i != End; i += Step)
        {
            bool Found = true;
            for (int32 j = 0; j < SubStrLen; ++j)
            {
                if (!CompareFunc(StrPtr[i + j], SubStrPtr[j]))
                {
                    Found = false;
                    break;
                }
            }
            if (Found)
            {
                return i;
            }
        }
        return INDEX_NONE;
    };

    if (SearchDir == ESearchDir::FromStart)
    {
        StartPosition = FMath::Clamp(StartPosition, 0, StrLen - SubStrLen);
        return FindSubString(StartPosition, StrLen - SubStrLen + 1, 1);
    }
    else // ESearchDir::FromEnd
    {
        StartPosition = (StartPosition == INDEX_NONE) ? StrLen - SubStrLen : FMath::Min(StartPosition, StrLen - SubStrLen);
        return FindSubString(StartPosition, -1, -1);
    }
}
