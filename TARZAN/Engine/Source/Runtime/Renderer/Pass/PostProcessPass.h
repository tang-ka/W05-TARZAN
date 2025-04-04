#ifndef POSTPROCESSPASS_H
#define POSTPROCESSPASS_H

#include "RenderPass.h"

class PostProcessPass : public RenderPass {
public:
    virtual void Setup(ID3D11DeviceContext* context) override;
    virtual void Execute(ID3D11DeviceContext* context) override;
    virtual void Cleanup(ID3D11DeviceContext* context) override;
};

#endif // POSTPROCESSPASS_H
