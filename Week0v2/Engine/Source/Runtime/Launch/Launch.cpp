#include "Core/HAL/PlatformType.h"
#include "EditorEngine.h"
#include "LaunchEngineLoop.h"

UEditorEngine GEngine;
FEngineLoop GEngineLoop; 

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // 사용 안하는 파라미터들
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    GEngine.Init(hInstance);
    GEngine.Tick();
    GEngine.Exit();

    return 0;
}
