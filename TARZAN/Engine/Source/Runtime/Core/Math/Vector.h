#pragma once

#include <DirectXMath.h>

#include "Container/String.h"
#include <cmath>   // for pow
#include <cwctype> // for iswspace, iswdigit (만약 FString이 wchar_t 기반이라면)

struct FVector2D
{
	float x,y;
	FVector2D(float _x = 0, float _y = 0) : x(_x), y(_y) {}

	FVector2D operator+(const FVector2D& rhs) const
	{
		return FVector2D(x + rhs.x, y + rhs.y);
	}
	FVector2D operator-(const FVector2D& rhs) const
	{
		return FVector2D(x - rhs.x, y - rhs.y);
	}
	FVector2D operator*(float rhs) const
	{
		return FVector2D(x * rhs, y * rhs);
	}
	FVector2D operator/(float rhs) const
	{
		return FVector2D(x / rhs, y / rhs);
	}
	FVector2D& operator+=(const FVector2D& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
};

// 3D 벡터
struct FVector
{
    float x, y, z;
    FVector(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    FVector operator-(const FVector& other) const {
        return FVector(x - other.x, y - other.y, z - other.z);
    }
    FVector operator+(const FVector& other) const {
        return FVector(x + other.x, y + other.y, z + other.z);
    }

    // 벡터 내적
    float Dot(const FVector& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // 벡터 크기
    float Magnitude() const {
        return sqrt(x * x + y * y + z * z);
    }

    // 벡터 정규화
    FVector Normalize() const {
        float mag = Magnitude();
        return (mag > 0) ? FVector(x / mag, y / mag, z / mag) : FVector(0, 0, 0);
    }
    FVector Cross(const FVector& Other) const
    {
        return FVector{
            y * Other.z - z * Other.y,
            z * Other.x - x * Other.z,
            x * Other.y - y * Other.x
        };
    }
    // 스칼라 곱셈
    FVector operator*(float scalar) const {
        return FVector(x * scalar, y * scalar, z * scalar);
    }

    bool operator==(const FVector& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    float Distance(const FVector& other) const {
        // 두 벡터의 차 벡터의 크기를 계산
        return ((*this - other).Magnitude());
    }
    DirectX::XMFLOAT3 ToXMFLOAT3() const
    {
        return DirectX::XMFLOAT3(x, y, z);
    }

    // *** 추가된 ToString 함수 ***
    /**
     * @brief 벡터를 문자열로 변환합니다. (예: "X=1.2 Y=3.4 Z=5.6")
     * @return FString 형태의 벡터 표현 문자열.
     */
    FString ToString() const
    {
        // FString::Printf를 사용하여 포맷팅된 문자열 생성
        // TEXT() 매크로는 리터럴 문자열을 TCHAR 타입으로 만들어줍니다.
        return FString::Printf(TEXT("X=%f Y=%f Z=%f"), x, y, z);

        // 필요에 따라 소수점 정밀도 지정 가능: 예) "X=%.2f Y=%.2f Z=%.2f"
        // return FString::Printf(TEXT("X=%.2f Y=%.2f Z=%.2f"), x, y, z);
    }


    /**
     * @brief 주어진 문자열에서 벡터 값을 수동으로 파싱하여 설정합니다.
     *        "X=... Y=... Z=..." 형식을 기대하며, 순서는 X, Y, Z여야 합니다.
     * @param SourceString 파싱할 원본 FString (char 기반).
     * @return 파싱에 성공하면 true, 실패하면 false.
     */
    bool InitFromString(const FString& SourceString)
    {
        // FString에서 C-스타일 문자열 포인터 얻기 (메서드 이름은 실제 FString 구현에 맞게 조정)
        const char* currentPos = *SourceString; // 또는 SourceString.c_str();
        if (currentPos == nullptr) return false; // 빈 문자열 또는 오류

        float parsedX, parsedY, parsedZ;
        char* endPtrX = nullptr;
        char* endPtrY = nullptr;
        char* endPtrZ = nullptr;

        // 1. "X=" 찾기 및 값 파싱
        const char* xMarker = strstr(currentPos, "X=");
        if (xMarker == nullptr) return false; // "X=" 마커 없음

        // "X=" 다음 위치로 이동
        const char* xValueStart = xMarker + 2; // "X=" 길이만큼 이동

        // 숫자 변환 시도 (strtof는 선행 공백 무시)
        parsedX = strtof(xValueStart, &endPtrX);
        // 변환 실패 확인 (숫자를 전혀 읽지 못함)
        if (endPtrX == xValueStart) return false;

        // 파싱 성공, 다음 검색 시작 위치 업데이트
        currentPos = endPtrX;

        // 2. "Y=" 찾기 및 값 파싱 (X 이후부터 검색)
        const char* yMarker = strstr(currentPos, "Y=");
        if (yMarker == nullptr) return false; // "Y=" 마커 없음

        const char* yValueStart = yMarker + 2;
        parsedY = strtof(yValueStart, &endPtrY);
        if (endPtrY == yValueStart) return false; // 변환 실패

        // 다음 검색 시작 위치 업데이트
        currentPos = endPtrY;

        // 3. "Z=" 찾기 및 값 파싱 (Y 이후부터 검색)
        const char* zMarker = strstr(currentPos, "Z=");
        if (zMarker == nullptr) return false; // "Z=" 마커 없음

        const char* zValueStart = zMarker + 2;
        parsedZ = strtof(zValueStart, &endPtrZ);
        if (endPtrZ == zValueStart) return false; // 변환 실패

        // 모든 파싱 성공 시, 멤버 변수 업데이트
        x = parsedX;
        y = parsedY;
        z = parsedZ;
        return true;
    }


    static const FVector ZeroVector;
    static const FVector OneVector;
    static const FVector UpVector;
    static const FVector ForwardVector;
    static const FVector RightVector;
};
