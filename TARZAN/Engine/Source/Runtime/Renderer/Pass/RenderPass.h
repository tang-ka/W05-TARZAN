#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <d3d11.h>

class RenderPass {
public:
    virtual ~RenderPass() {}
    
    // 패스 실행 전 필요한 설정을 진행한다.
    virtual void Setup(ID3D11DeviceContext* context) = 0;
    
    // 패스의 핵심 드로우 호출을 진행한다.
    virtual void Execute(ID3D11DeviceContext* context) = 0;
    
    // 패스 종료 후 상태 복원 등 정리 작업을 수행한다.
    virtual void Cleanup(ID3D11DeviceContext* context) = 0;
};

#endif // RENDERPASS_H
