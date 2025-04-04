#include "GBufferPass.h"

// GBufferPass의 Setup: G-buffer 렌더 타겟 바인딩 및 클리어, 뷰포트 설정 등을 진행
void GBufferPass::Setup(ID3D11DeviceContext* context) {
    // 예시: G-buffer용 렌더 타겟을 바인딩하고, 해당 버퍼들을 클리어한다.
    // context->OMSetRenderTargets(...);
    // context->ClearRenderTargetView(...);
}

// GBufferPass의 Execute: 모든 기하 정보를 G-buffer에 기록하는 Draw 호출 수행
void GBufferPass::Execute(ID3D11DeviceContext* context) {
    // 예시: 씬의 모든 오브젝트를 그려서, 위치, 노멀, 알베도 등의 정보를 G-buffer에 기록한다.
    // for (오브젝트마다) { context->DrawIndexed(...); }
}

// GBufferPass의 Cleanup: 상태 복원 등이 필요한 경우 정리 작업 수행
void GBufferPass::Cleanup(ID3D11DeviceContext* context) {
    // 예시: 이전의 렌더 타겟 상태를 복원
}
