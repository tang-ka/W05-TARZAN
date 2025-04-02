#pragma once
#include "Core/Hal/PlatformType.h"
#include "EngineBaseTypes.h"
#include "EditorEngine.h"

extern UEditorEngine* GEngine;

class UEditorStateManager
{
public:
    EEditorState GetCurrentState() const { return CurrentState; }
    void SetState(EEditorState NewState);
    bool IsPIERunning() 
    { 
        return 
        CurrentState == EEditorState::Playing || 
        CurrentState == EEditorState::Paused || 
        CurrentState == EEditorState::Resuming; 
    }

private:
    bool IsValidTransition(EEditorState To);

private:
    EEditorState CurrentState;
};

inline void UEditorStateManager::SetState(EEditorState NewState)
{
    if (!IsValidTransition(NewState))
        return;

    CurrentState = NewState;

    switch (NewState)
    {
    case EEditorState::Editing:
        break;

    case EEditorState::PreparingPlay:       // Connect to play button
        GEngine->PreparePIE();           // 추후 Prepare에 실패했을 때 고려 할 수 있어야 할듯
        SetState(EEditorState::Playing);
        return;

    case EEditorState::Playing:             // auto Transition
        GEngine->StartPIE();
        break;

    case EEditorState::Paused:              // Connect to pause button
        GEngine->PausedPIE();
        break;

    case EEditorState::Resuming:            // Connect to resume button
        GEngine->ResumingPIE();
        break;

    case EEditorState::Stopped:             // Connect to stop button
        GEngine->StopPIE();
        SetState(EEditorState::Editing);
        return;
    }
}

inline bool UEditorStateManager::IsValidTransition(EEditorState To)
{
    switch (CurrentState)
    {
    case EEditorState::Editing:
        return To == EEditorState::PreparingPlay;

    case EEditorState::PreparingPlay:
        return To == EEditorState::Playing;

    case EEditorState::Playing:
        return To == EEditorState::Paused || To == EEditorState::Stopped;

    case EEditorState::Paused:
        return To == EEditorState::Resuming || To == EEditorState::Stopped;

    case EEditorState::Resuming:
        return To == EEditorState::Playing || To == EEditorState::Stopped;

    case EEditorState::Stopped:
        return To == EEditorState::Editing;
    }
}

