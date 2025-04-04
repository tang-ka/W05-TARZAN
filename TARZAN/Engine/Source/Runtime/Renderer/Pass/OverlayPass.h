#ifndef OVERLAYPASS_H
#define OVERLAYPASS_H

#include "RenderPass.h"

class OverlayPass : public RenderPass {
public:
    virtual void Setup(ID3D11DeviceContext* context) override;
    virtual void Execute(ID3D11DeviceContext* context) override;
    virtual void Cleanup(ID3D11DeviceContext* context) override;
};

#endif // OVERLAYPASS_H
