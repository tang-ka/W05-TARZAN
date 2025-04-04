#include "OverlayPass.h"

void OverlayPass::Setup(ID3D11DeviceContext* context) {
    // 예시: UI 렌더 타겟(또는 백 버퍼)을 설정하고, 알파 블렌딩 상태 등 UI 렌더링에 필요한 상태를 설정
}

void OverlayPass::Execute(ID3D11DeviceContext* context) {
    // 예시: ImGui, 디버그 텍스트 등 UI 요소를 렌더링하는 Draw 호출
}

void OverlayPass::Cleanup(ID3D11DeviceContext* context) {
    // 예시: UI 렌더링 후 상태 복원
}
