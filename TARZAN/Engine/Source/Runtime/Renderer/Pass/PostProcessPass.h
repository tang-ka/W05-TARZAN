#ifndef POSTPROCESSPASS_H
#define POSTPROCESSPASS_H

#include "RenderPass.h"

class PostProcessPass : public RenderPass 
{
public:
    virtual void Setup(ID3D11DeviceContext* context) override;
    virtual void Execute(ID3D11DeviceContext* context) override;
    virtual void Cleanup(ID3D11DeviceContext* context) override;
};

inline void PostProcessPass::Setup(ID3D11DeviceContext* context) 
{
    // 예시: 후처리 렌더 타겟과 관련 셰이더, 상수 버퍼 등을 설정
}

inline void PostProcessPass::Execute(ID3D11DeviceContext* context) 
{
    // 예시: 색 보정, 블룸, SSAO 등의 후처리 효과를 적용하는 Draw 호출
}

inline void PostProcessPass::Cleanup(ID3D11DeviceContext* context) 
{
    // 예시: 후처리 상태 복원
}
#endif // POSTPROCESSPASS_H
