#include "UHeightFogComponent.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Launch/EditorEngine.h"
#include "Editor/LevelEditor/SLevelEditor.h"
#include "PropertyEditor/ShowFlags.h"

extern UEditorEngine* GEngine;

UHeightFogComponent::UHeightFogComponent()
{ 
    FogDensity = 1.00f;
    FogHeightFalloff = 0.1f;
    StartDistance = 5.0f;
    FogCutoffDistance = 800.0f;
    FogMaxOpacity = 1.0f;
    FogInscatteringColor = FLinearColor(0.6f, 0.1f, 0.1f, 1.0f); // 어두운 빨강

    std::shared_ptr<FEditorViewportClient> ActiveViewport = GEngine->GetLevelEditor()->GetActiveViewportClient();
    DisableFog = (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Fog)) ? 0 : 1;
}
UHeightFogComponent::~UHeightFogComponent()
{
}
void UHeightFogComponent::InitializeComponent()
{
    Super::InitializeComponent();
}
void UHeightFogComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}
