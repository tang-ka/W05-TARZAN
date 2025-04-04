#ifndef LIGHTINGPASS_H
#define LIGHTINGPASS_H

#include "RenderPass.h"

class LightingPass : public RenderPass {
public:
    virtual void Setup(ID3D11DeviceContext* context) override;
    virtual void Execute(ID3D11DeviceContext* context) override;
    virtual void Cleanup(ID3D11DeviceContext* context) override;
};

#endif // LIGHTINGPASS_H
