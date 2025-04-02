#include "LaunchEngineLoop.h"

#include "EditorEngine.h"

UEditorEngine GEngine;

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
    GEngine.Init(hInstance);
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
