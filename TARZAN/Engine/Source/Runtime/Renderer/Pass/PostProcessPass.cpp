#include "PostProcessPass.h"

void PostProcessPass::Setup(ID3D11DeviceContext* context) {
    // 예시: 후처리 렌더 타겟과 관련 셰이더, 상수 버퍼 등을 설정
}

void PostProcessPass::Execute(ID3D11DeviceContext* context) {
    // 예시: 색 보정, 블룸, SSAO 등의 후처리 효과를 적용하는 Draw 호출
}

void PostProcessPass::Cleanup(ID3D11DeviceContext* context) {
    // 예시: 후처리 상태 복원
}
