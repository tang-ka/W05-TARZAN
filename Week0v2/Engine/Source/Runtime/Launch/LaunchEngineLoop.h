#pragma once
#include "HAL/PlatformType.h"

class FEngineLoop
{
public:
    FEngineLoop();
    virtual ~FEngineLoop();
    int32 PreInit();
    int32 Init();
    void Tick();
    void Exit();

private:
    // TODO : 이후에 게임 엔진이 필요할 시 따로 관리 될 예정
    bool bIsEditor = true;
};
