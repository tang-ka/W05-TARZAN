#pragma once

// 쿼터니언
struct FQuat
{
	float w, x, y, z;

	// 기본 생성자
	FQuat() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}

	// FQuat 생성자 추가: 회전 축과 각도를 받아서 FQuat 생성
	FQuat(const FVector& Axis, float Angle)
	{
		float HalfAngle = Angle * 0.5f;
		float SinHalfAngle = sinf(HalfAngle);
		float CosHalfAngle = cosf(HalfAngle);

		x = Axis.x * SinHalfAngle;
		y = Axis.y * SinHalfAngle;
		z = Axis.z * SinHalfAngle;
		w = CosHalfAngle;
	}

	// w, x, y, z 값으로 초기화
	FQuat(float InW, float InX, float InY, float InZ) : w(InW), x(InX), y(InY), z(InZ) {}

	// 쿼터니언의 곱셈 연산 (회전 결합)
	FQuat operator*(const FQuat& Other) const
	{
		return FQuat(
			w * Other.w - x * Other.x - y * Other.y - z * Other.z,
			w * Other.x + x * Other.w + y * Other.z - z * Other.y,
			w * Other.y - x * Other.z + y * Other.w + z * Other.x,
			w * Other.z + x * Other.y - y * Other.x + z * Other.w
		);
	}

	// (쿼터니언) 벡터 회전
	FVector RotateVector(const FVector& Vec) const
	{
		// 벡터를 쿼터니언으로 변환
		FQuat vecQuat(0.0f, Vec.x, Vec.y, Vec.z);
		// 회전 적용 (q * vec * q^-1)
		FQuat conjugate = FQuat(w, -x, -y, -z);  // 쿼터니언의 켤레
		FQuat result = *this * vecQuat * conjugate;

		return FVector(result.x, result.y, result.z); // 회전된 벡터 반환
	}

	// 단위 쿼터니언 여부 확인
	bool IsNormalized() const
	{
		return fabs(w * w + x * x + y * y + z * z - 1.0f) < 1e-6f;
	}

	// 쿼터니언 정규화 (단위 쿼터니언으로 만듬)
	FQuat Normalize() const
	{
		float magnitude = sqrtf(w * w + x * x + y * y + z * z);
		return FQuat(w / magnitude, x / magnitude, y / magnitude, z / magnitude);
	}

	// 회전 각도와 축으로부터 쿼터니언 생성 (axis-angle 방식)
	static FQuat FromAxisAngle(const FVector& Axis, float Angle)
	{
		float halfAngle = Angle * 0.5f;
		float sinHalfAngle = sinf(halfAngle);
		return FQuat(cosf(halfAngle), Axis.x * sinHalfAngle, Axis.y * sinHalfAngle, Axis.z * sinHalfAngle);
	}

	static FQuat CreateRotation(float roll, float pitch, float yaw)
	{
		// 각도를 라디안으로 변환
		float radRoll = roll * (3.14159265359f / 180.0f);
		float radPitch = pitch * (3.14159265359f / 180.0f);
		float radYaw = yaw * (3.14159265359f / 180.0f);

		// 각 축에 대한 회전 쿼터니언 계산
		FQuat qRoll = FQuat(FVector(1.0f, 0.0f, 0.0f), radRoll);  // X축 회전
		FQuat qPitch = FQuat(FVector(0.0f, 1.0f, 0.0f), radPitch);  // Y축 회전
		FQuat qYaw = FQuat(FVector(0.0f, 0.0f, 1.0f), radYaw);  // Z축 회전

		// 회전 순서대로 쿼터니언 결합 (Y -> X -> Z)
		return qRoll * qPitch * qYaw;
	}
	// 쿼터니언을 회전 행렬로 변환
	FMatrix ToMatrix() const
	{
		FMatrix RotationMatrix;
		RotationMatrix.M[0][0] = 1.0f - 2.0f * (y * y + z * z);
		RotationMatrix.M[0][1] = 2.0f * (x * y - w * z);
		RotationMatrix.M[0][2] = 2.0f * (x * z + w * y);
		RotationMatrix.M[0][3] = 0.0f;


		RotationMatrix.M[1][0] = 2.0f * (x * y + w * z);
		RotationMatrix.M[1][1] = 1.0f - 2.0f * (x * x + z * z);
		RotationMatrix.M[1][2] = 2.0f * (y * z - w * x);
		RotationMatrix.M[1][3] = 0.0f;

		RotationMatrix.M[2][0] = 2.0f * (x * z - w * y);
		RotationMatrix.M[2][1] = 2.0f * (y * z + w * x);
		RotationMatrix.M[2][2] = 1.0f - 2.0f * (x * x + y * y);
		RotationMatrix.M[2][3] = 0.0f;

		RotationMatrix.M[3][0] = RotationMatrix.M[3][1] = RotationMatrix.M[3][2] = 0.0f;
		RotationMatrix.M[3][3] = 1.0f;

		return RotationMatrix;
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
};
