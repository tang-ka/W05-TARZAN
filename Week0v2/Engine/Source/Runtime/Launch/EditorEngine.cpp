#include "EditorEngine.h"
#include "ImGuiManager.h"
#include "Engine/World.h"
#include "PropertyEditor/ViewportTypePanel.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/UnrealEd.h"
#include "UnrealClient.h"
#include "GameFramework/Actor.h"
#include "slate/Widgets/Layout/SSplitter.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/SceneMgr.h"


class ULevel;

FGraphicsDevice UEditorEngine::graphicDevice;
FRenderer UEditorEngine::renderer;
FResourceMgr UEditorEngine::resourceMgr;

UEditorEngine::UEditorEngine()
    : hWnd(nullptr)
    , UIMgr(nullptr)
    , GWorld(nullptr)
    , LevelEditor(nullptr)
    , UnrealEditor(nullptr)
{
}


int32 UEditorEngine::Init(HWND hwnd)
{
    /* must be initialized before window. */
    hWnd = hwnd;
    graphicDevice.Initialize(hWnd);
    renderer.Initialize(&graphicDevice);
    UIMgr = new UImGuiManager;
    UIMgr->Initialize(hWnd, graphicDevice.Device, graphicDevice.DeviceContext);
    resourceMgr.Initialize(&renderer, &graphicDevice);

    
    FWorldContext EditorContext;
    EditorContext.WorldType = EWorldType::Editor;
    std::shared_ptr<UWorld> EditWorld = EditorContext.World();
    EditWorld = std::shared_ptr<UWorld>(FObjectFactory::ConstructObject<UWorld>());
    EditWorld->Initialize();
    GWorld = EditWorld;
    worldContexts.Add(EditorContext);
    
    FWorldContext PIEContext;
    EditorContext.WorldType = EWorldType::PIE;
    worldContexts.Add(PIEContext);
    
    UnrealEditor = new UnrealEd();
    UnrealEditor->Initialize();
    
    LevelEditor = new SLevelEditor();
    LevelEditor->Initialize();
    
    SceneMgr = new FSceneMgr();

    return 0;
}


void UEditorEngine::Render()
{
    graphicDevice.Prepare();
    if (LevelEditor->IsMultiViewport())
    {
        std::shared_ptr<FEditorViewportClient> viewportClient = GetLevelEditor()->GetActiveViewportClient();
        for (int i = 0; i < 4; ++i)
        {
            LevelEditor->SetViewportClient(i);
            renderer.PrepareRender();
            renderer.Render(GWorld.get(),LevelEditor->GetActiveViewportClient());
        }
        GetLevelEditor()->SetViewportClient(viewportClient);
    }
    else
    {
        renderer.PrepareRender();
        renderer.Render(GWorld.get(),LevelEditor->GetActiveViewportClient());
    }
}

void UEditorEngine::Tick(float deltaSeconds)
{
    for (FWorldContext& WorldContext : worldContexts)
    {
        std::shared_ptr<UWorld> EditorWorld = WorldContext.World();
        if (EditorWorld && WorldContext.WorldType == EWorldType::Editor)
        {
            ULevel* Level = EditorWorld->GetLevel();
            {
                for (AActor* Actor : Level->GetActors())
                {
                    // if (Actor && Actor->bTickInEditor)
                    // {
                    //     Actor->Tick(deltaSeconds);
                    // }
                }
            }
        }
        else if (EditorWorld && WorldContext.WorldType == EWorldType::PIE)
        {
            ULevel* Level = EditorWorld->GetLevel();
            {
                for (AActor* Actor : Level->GetActors())
                {
                    if (Actor)
                    {
                        Actor->Tick(deltaSeconds);
                    }
                }
            }
        }
    }
    Input();
    GWorld->Tick(deltaSeconds);
    LevelEditor->Tick(deltaSeconds);
    Render();
    UIMgr->BeginFrame();
    UnrealEditor->Render();

    Console::GetInstance().Draw();

    UIMgr->EndFrame();

    // Pending 처리된 오브젝트 제거

    // TODO : 이거 잘 안되는 것 이유 파악 
    // GUObjectArray.ProcessPendingDestroyObjects();

    graphicDevice.SwapBuffer();
}

float UEditorEngine::GetAspectRatio(IDXGISwapChain* swapChain) const
{
    DXGI_SWAP_CHAIN_DESC desc;
    swapChain->GetDesc(&desc);
    return static_cast<float>(desc.BufferDesc.Width) / static_cast<float>(desc.BufferDesc.Height);
}

void UEditorEngine::Input()
{
    if (GetAsyncKeyState('M') & 0x8000)
    {
        if (!bTestInput)
        {
            bTestInput = true;
            if (LevelEditor->IsMultiViewport())
            {
                LevelEditor->OffMultiViewport();
            }
            else
                LevelEditor->OnMultiViewport();
        }
    }
    else
    {
        bTestInput = false;
    }
}


void UEditorEngine::PreparePIE()
{
    // 1. World 복제
    UWorld* EditorWorld = GetEditorWorldContext()->World().get();
    //FDuplicationMap DupMap;
    //UWorld* PIEWorld = EditorWorld->DuplicateSubObjects(DupMap);
    UWorld* PIEWorld = UWorld::DuplicateWorldForPIE(EditorWorld);
    PIEWorld->CreateBaseObject();

    GWorld = std::shared_ptr<UWorld>(PIEWorld);

    // 2. 복제한 World Type PIE로 변경
    //PIEWorld->SetType(EWorldType::PIE);
}

void UEditorEngine::StartPIE()
{
    // 1. BeingPlay() 호출
    GWorld->BeginPlay();
}

void UEditorEngine::PausedPIE()
{

}

void UEditorEngine::ResumingPIE()
{

}

void UEditorEngine::StopPIE()
{
    // 1. World Clear
    if (GWorld && GWorld->IsPIEWorld())
    {
        GWorld->ClearScene();
    }

    GWorld = GetEditorWorldContext()->World();
}

void UEditorEngine::Exit()
{
    LevelEditor->Release();
    GWorld->Release();
    UIMgr->Shutdown();
    delete UIMgr;
    delete SceneMgr;
    resourceMgr.Release(&renderer);
    renderer.Release();
    graphicDevice.Release();
}


