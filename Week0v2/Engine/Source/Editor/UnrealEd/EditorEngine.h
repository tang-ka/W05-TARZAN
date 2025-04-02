#pragma once
#include "Engine/Engine.h"

class UEditorEngine : UEngine
{
public:
    virtual void Tick(float deltaSecondes) override;
};
