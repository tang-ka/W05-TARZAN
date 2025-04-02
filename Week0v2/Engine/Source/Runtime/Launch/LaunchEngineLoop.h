#pragma once
#include "HAL/PlatformType.h"


class FEngineLoop
{
public:
    FEngineLoop();
    virtual ~FEngineLoop();
    int32 PreInit();
    int32 Init(HINSTANCE hInstance);
    void Tick();
    void Exit();

    void WindowInit(HINSTANCE hInstance);

    HWND hWnd;
private:
    // TODO : 이후에 게임 엔진이 필요할 시 따로 관리 될 예정
    bool bIsEditor = true;
    bool bIsExit = false;
    const int32 targetFPS = 60;
};

extern  FEngineLoop GEngineLoop;
