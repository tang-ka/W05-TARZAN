#include "LaunchEngineLoop.h"

#include "EditorEngine.h"
#include "LevelEditor/SLevelEditor.h"
#include "PropertyEditor/ViewportTypePanel.h"
#include "UnrealEd/UnrealEd.h"

UEditorEngine GEngine;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }
    int zDelta = 0;
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            //UGraphicsDevice 객체의 OnResize 함수 호출
            if (UEditorEngine::graphicDevice.SwapChain)
            {
                UEditorEngine::graphicDevice.OnResize(hWnd);
            }
            for (int i = 0; i < 4; i++)
            {
                if (GEngine.GetLevelEditor())
                {
                    if (GEngine.GetLevelEditor()->GetViewports()[i])
                    {
                        GEngine.GetLevelEditor()->GetViewports()[i]->ResizeViewport(UEditorEngine::graphicDevice.SwapchainDesc);
                    }
                }
            }
        }
     Console::GetInstance().OnResize(hWnd);
        if (GEngine.GetUnrealEditor())
        {
            GEngine.GetUnrealEditor()->OnResize(hWnd);
        }
        ViewportTypePanel::GetInstance().OnResize(hWnd);
        break;
    case WM_MOUSEWHEEL:
        if (ImGui::GetIO().WantCaptureMouse)
            return 0;
        zDelta = GET_WHEEL_DELTA_WPARAM(wParam); // 휠 회전 값 (+120 / -120)
        if (GEngine.GetLevelEditor())
        {
            if (GEngine.GetLevelEditor()->GetActiveViewportClient()->IsPerspective())
            {
                if (GEngine.GetLevelEditor()->GetActiveViewportClient()->GetIsOnRBMouseClick())
                {
                    GEngine.GetLevelEditor()->GetActiveViewportClient()->SetCameraSpeedScalar(
                        static_cast<float>(GEngine.GetLevelEditor()->GetActiveViewportClient()->GetCameraSpeedScalar() + zDelta * 0.01)
                    );
                }
                else
                {
                    GEngine.GetLevelEditor()->GetActiveViewportClient()->CameraMoveForward(zDelta * 0.1f);
                }
            }
            else
            {
                FEditorViewportClient::SetOthoSize(-zDelta * 0.01f);
            }
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}


FEngineLoop::FEngineLoop()
{
}

FEngineLoop::~FEngineLoop()
{
}

int32 FEngineLoop::PreInit()
{
    return 0;
}

int32 FEngineLoop::Init(HINSTANCE hInstance)
{
    WindowInit(hInstance);
    GEngine.Init(hWnd);
    return 0;
}

void FEngineLoop::Tick()
{
    GEngine.Tick();
}

void FEngineLoop::Exit()
{
    GEngine.Exit();
}

void FEngineLoop::WindowInit(HINSTANCE hInstance)
{
    WCHAR WindowClass[] = L"JungleWindowClass";

    WCHAR Title[] = L"Game Tech Lab";

    WNDCLASSW wndclass = {0};
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.lpszClassName = WindowClass;

    RegisterClassW(&wndclass);

    hWnd = CreateWindowExW(
        0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
        nullptr, nullptr, hInstance, nullptr
    );
}
