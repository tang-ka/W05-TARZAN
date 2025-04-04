#ifndef GBUFFERPASS_H
#define GBUFFERPASS_H

#include "RenderPass.h"

class GBufferPass : public RenderPass {
public:
    virtual void Setup(ID3D11DeviceContext* context) override;
    virtual void Execute(ID3D11DeviceContext* context) override;
    virtual void Cleanup(ID3D11DeviceContext* context) override;
};

#endif // GBUFFERPASS_H
