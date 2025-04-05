#include "Renderer.h"
#include <d3dcompiler.h>

#include "Engine/World.h"
#include "Actors/Player.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/UBillboardComponent.h"
#include "Components/UParticleSubUVComp.h"
#include "Components/UText.h"
#include "Components/Material/Material.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Launch/EditorEngine.h"
#include "Math/JungleMath.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "UObject/Casts.h"
#include "UObject/Object.h"
#include "PropertyEditor/ShowFlags.h"
#include "UObject/UObjectIterator.h"
#include "Components/SkySphereComponent.h"
#include "FireballComp.h"
#include "Renderer/Pass/GBufferPass.h"
#include "Renderer/Pass/LightingPass.h"
#include "Renderer/Pass/PostProcessPass.h"
#include "Renderer/Pass/OverlayPass.h"

#include "Editor/LevelEditor/SLevelEditor.h"
#include "Runtime/Launch/ImGuiManager.h"
#include "UnrealEd/UnrealEd.h"
#include "UHeightFogComponent.h"
#include "LevelEditor/SLevelEditor.h"

extern UEditorEngine* GEngine;

FRenderer::~FRenderer()
{
    for (RenderPass* Pass : Passes)
        delete Pass;

    Passes.Empty();
}

void FRenderer::Initialize(FGraphicsDevice* graphics)
{
    Graphics = graphics;
    RenderResourceManager.Initialize(Graphics->Device);
    ShaderManager.Initialize(Graphics->Device, Graphics->DeviceContext);
    ConstantBufferUpdater.Initialize(Graphics->DeviceContext);

    CreateShader();
    CreateConstantBuffer();
    ConstantBufferUpdater.UpdateLitUnlitConstant(FlagBuffer, 1);

    auto world = GEngine->GetWorld();

    //UIMgr = new UImGuiManager;
    //UIMgr->Initialize(hWnd, graphicDevice.Device, graphicDevice.DeviceContext);

    Passes.Add(new GBufferPass());
    Passes.Add(new LightingPass());
    Passes.Add(new PostProcessPass());
    Passes.Add(new OverlayPass());

    // test
    CreateDummyPostProcessResources();
}

void FRenderer::Render()
{
    RenderGBuffer();

    RenderLightPass();
    
    RenderPostProcessPass();

    RenderOverlayPass();

    //for (RenderPass* pass : Passes)
    //{
    //    pass->Setup(Context);
    //    pass->Execute(Context);
    //    pass->Cleanup(Context);
    //}
}

void FRenderer::Release()
{
    ReleaseShader();
    ReleaseConstantBuffer();
}

#pragma region Shader
void FRenderer::CreateShader()
{
    // 기본 셰이더 설정
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"MATERIAL_INDEX", 0, DXGI_FORMAT_R32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    ShaderManager.CreateVertexShader(
        L"Shaders/StaticMeshVertexShader.hlsl", "mainVS",
        VertexShader, layout, ARRAYSIZE(layout), &InputLayout, &Stride, sizeof(FVertexSimple));

    ShaderManager.CreatePixelShader(
        L"Shaders/StaticMeshPixelShader.hlsl", "mainPS",
        PixelShader);

    ShaderManager.CreatePixelShader(
        L"Shaders/PostProcessPixelShader.hlsl", "mainPS", PostProcessPixelShader);

    // 텍스쳐 셰이더 설정
    D3D11_INPUT_ELEMENT_DESC textureLayout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    ShaderManager.CreateVertexShader(
        L"Shaders/VertexTextureShader.hlsl", "main",
        VertexTextureShader, textureLayout, ARRAYSIZE(textureLayout), &TextureInputLayout, &TextureStride, sizeof(FVertexTexture));

    ShaderManager.CreatePixelShader(
        L"Shaders/PixelTextureShader.hlsl", "main",
        PixelTextureShader);

    // 라인 셰이더 설정
    ShaderManager.CreateVertexShader(
        L"Shaders/ShaderLine.hlsl", "mainVS",
        VertexLineShader, nullptr, 0); // 라인 셰이더는 Layout 안 쓰면 nullptr 전달

    ShaderManager.CreatePixelShader(
        L"Shaders/ShaderLine.hlsl", "mainPS",
        PixelLineShader);
}

void FRenderer::ReleaseShader()
{
    ShaderManager.ReleaseShader(InputLayout, VertexShader, PixelShader);
    ShaderManager.ReleaseShader(TextureInputLayout, VertexTextureShader, PixelTextureShader);
    ShaderManager.ReleaseShader(nullptr, VertexLineShader, PixelLineShader);
}


// Prepare
void FRenderer::PrepareShader() const
{
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(InputLayout);

    if (ConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &ConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &MaterialConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(2, 1, &LightingBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(3, 1, &FlagBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(4, 1, &SubMeshConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(5, 1, &TextureConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(6, 1, &FireballConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(7, 1, &FogConstantBuffer);
    }
}

void FRenderer::PrepareTextureShader() const
{
    Graphics->DeviceContext->VSSetShader(VertexTextureShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelTextureShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(TextureInputLayout);

    if (ConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
    }
}

void FRenderer::PrepareSubUVConstant() const
{
    if (SubUVConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(1, 1, &SubUVConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &SubUVConstantBuffer);
    }
}

void FRenderer::PrepareLineShader() const
{
    Graphics->DeviceContext->VSSetShader(VertexLineShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelLineShader, nullptr, 0);

    if (ConstantBuffer && GridConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);     // MatrixBuffer (b0)
        Graphics->DeviceContext->VSSetConstantBuffers(1, 1, &GridConstantBuffer); // GridParameters (b1)
        Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &GridConstantBuffer);
        Graphics->DeviceContext->VSSetConstantBuffers(3, 1, &LinePrimitiveBuffer);

        Graphics->DeviceContext->VSSetShaderResources(2, 1, &pBBSRV);
        Graphics->DeviceContext->VSSetShaderResources(3, 1, &pConeSRV);
        Graphics->DeviceContext->VSSetShaderResources(4, 1, &pOBBSRV);

    }
}
#pragma endregion Shader

#pragma region ConstantBuffer
// ConstantBuffer
void FRenderer::CreateConstantBuffer()
{
    ConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FConstants));
    SubUVConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FSubUVConstant));
    GridConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FGridParameters));
    LinePrimitiveBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FPrimitiveCounts));
    MaterialConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FMaterialConstants));
    SubMeshConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FSubMeshConstants));
    TextureConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FTextureConstants));
    LightingBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FLighting));
    FlagBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FLitUnlitConstants));
    FireballConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FFireballArrayInfo));
    FogConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FFogConstants));
}

void FRenderer::ReleaseConstantBuffer()
{
    RenderResourceManager.ReleaseBuffer(ConstantBuffer);
    RenderResourceManager.ReleaseBuffer(SubUVConstantBuffer);
    RenderResourceManager.ReleaseBuffer(GridConstantBuffer);
    RenderResourceManager.ReleaseBuffer(LinePrimitiveBuffer);
    RenderResourceManager.ReleaseBuffer(MaterialConstantBuffer);
    RenderResourceManager.ReleaseBuffer(SubMeshConstantBuffer);
    RenderResourceManager.ReleaseBuffer(TextureConstantBuffer);
    RenderResourceManager.ReleaseBuffer(LightingBuffer);
    RenderResourceManager.ReleaseBuffer(FlagBuffer);
    RenderResourceManager.ReleaseBuffer(FireballConstantBuffer);
    RenderResourceManager.ReleaseBuffer(FogConstantBuffer);
}
#pragma endregion ConstantBuffer

#pragma region

void FRenderer::ClearRenderArr()
{
    StaticMeshObjs.Empty();
    GizmoObjs.Empty();
    BillboardObjs.Empty();
    LightObjs.Empty();
    FireballObjs.Empty();
}

void FRenderer::PrepareRender()
{
    if (GEngine->GetWorld()->WorldType == EWorldType::Editor)
    {
        for (const auto iter : TObjectRange<USceneComponent>())
        {
                UE_LOG(LogLevel::Display, "%d", GUObjectArray.GetObjectItemArrayUnsafe().Num());
                if (UStaticMeshComponent* pStaticMeshComp = Cast<UStaticMeshComponent>(iter))
                {
                    if (!Cast<UGizmoBaseComponent>(iter))
                        StaticMeshObjs.Add(pStaticMeshComp);
                }
                if (UGizmoBaseComponent* pGizmoComp = Cast<UGizmoBaseComponent>(iter))
                {
                    GizmoObjs.Add(pGizmoComp);
                }
                if (UBillboardComponent* pBillboardComp = Cast<UBillboardComponent>(iter))
                {
                    BillboardObjs.Add(pBillboardComp);
                }
                if (ULightComponentBase* pLightComp = Cast<ULightComponentBase>(iter))
                {
                    LightObjs.Add(pLightComp);
                }
                if (UFireballComponent* pFireComp = Cast<UFireballComponent>(iter))
                {
                    FireballObjs.Add(pFireComp);
                }
                if (UHeightFogComponent* HeightFog = Cast<UHeightFogComponent>(iter))
                {
                    fogData.FogDensity = HeightFog->GetFogDensity();
                    fogData.FogHeightFalloff = HeightFog->GetFogHeightFalloff();
                    fogData.StartDistance = HeightFog->GetStartDistance();
                    fogData.FogCutoffDistance = HeightFog->GetFogCutoffDistance();
                    fogData.FogMaxOpacity = HeightFog->GetFogMaxOpacity();
                    fogData.FogInscatteringColor = HeightFog->GetFogInscatteringColor();
                    fogData.CameraPosition = GEngine->GetLevelEditor()->GetActiveViewportClient()->GetCameraLocation();
                    ConstantBufferUpdater.UpdateFogConstant(FogConstantBuffer, fogData);
                } 
        }
    }
    else if (GEngine->GetWorld()->WorldType == EWorldType::PIE)
    {
        // UE_LOG(LogLevel::Display, "%d", GEngine->GetWorld()->GetActors().Num() );
        for (const auto iter : GEngine->GetWorld()->GetActors())
        {
            
            for (const auto iter2 : iter->GetComponents())
            {
                if (UStaticMeshComponent* pStaticMeshComp = Cast<UStaticMeshComponent>(iter2))
                {
                    if (!Cast<UGizmoBaseComponent>(iter2))
                        StaticMeshObjs.Add(pStaticMeshComp);
                }
                if (UBillboardComponent* pBillboardComp = Cast<UBillboardComponent>(iter2))
                {
                    BillboardObjs.Add(pBillboardComp);
                }
                if (ULightComponentBase* pLightComp = Cast<ULightComponentBase>(iter2))
                {
                    LightObjs.Add(pLightComp);
                }
                if (UFireballComponent* pFireComp = Cast<UFireballComponent>(iter))
                {
                    FireballObjs.Add(pFireComp);
                }
            }
        }
    }
}

void FRenderer::Render(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    Graphics->DeviceContext->RSSetViewports(1, &ActiveViewport->GetD3DViewport());
    Graphics->ChangeRasterizer(ActiveViewport->GetViewMode());
    ChangeViewMode(ActiveViewport->GetViewMode());
    ConstantBufferUpdater.UpdateLightConstant(LightingBuffer);
    if (FireballObjs.Num() > 0) {
        FFireballArrayInfo fireballArrayInfo;
        fireballArrayInfo.FireballCount = 0;
        for (int i = 0; i < FireballObjs.Num(); i++)
        {
            if (FireballObjs[i] != nullptr)
            {
                FFireballInfo fireballInfo = FireballObjs[i]->GetFireballInfo();
                fireballArrayInfo.FireballConstants[i].Intensity = fireballInfo.Intensity;
                fireballArrayInfo.FireballConstants[i].Radius = fireballInfo.Radius;
                fireballArrayInfo.FireballConstants[i].Color = fireballInfo.Color;
                fireballArrayInfo.FireballConstants[i].RadiusFallOff = fireballInfo.RadiusFallOff;
                fireballArrayInfo.FireballConstants[i].Position = FireballObjs[i]->GetWorldLocation();
                fireballArrayInfo.FireballCount++;
            }
        }
        ConstantBufferUpdater.UpdateFireballConstant(FireballConstantBuffer, fireballArrayInfo);
    }
    UPrimitiveBatch::GetInstance().RenderBatch(ConstantBuffer, ActiveViewport->GetViewMatrix(), ActiveViewport->GetProjectionMatrix());

    if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Primitives))
        RenderStaticMeshes(World, ActiveViewport);
    RenderGizmos(World, ActiveViewport);
    if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_BillboardText))
        RenderBillboards(World, ActiveViewport);
    RenderLight(World, ActiveViewport);

    ClearRenderArr();
}

void FRenderer::RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices) const
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
    Graphics->DeviceContext->Draw(numVertices, 0);
}

void FRenderer::RenderPrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices) const
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &Stride, &offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    Graphics->DeviceContext->DrawIndexed(numIndices, 0, 0);
}

void FRenderer::RenderPrimitive(OBJ::FStaticMeshRenderData* renderData, TArray<FStaticMaterial*> materials, TArray<UMaterial*> overrideMaterial, int selectedSubMeshIndex = -1) const
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &renderData->VertexBuffer, &Stride, &offset);

    if (renderData->IndexBuffer)
        Graphics->DeviceContext->IASetIndexBuffer(renderData->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    if (renderData->MaterialSubsets.Num() == 0)
    {
        // no submesh
        Graphics->DeviceContext->DrawIndexed(renderData->Indices.Num(), 0, 0);
    }

    for (int subMeshIndex = 0; subMeshIndex < renderData->MaterialSubsets.Num(); subMeshIndex++)
    {
        int materialIndex = renderData->MaterialSubsets[subMeshIndex].MaterialIndex;

        subMeshIndex == selectedSubMeshIndex ? ConstantBufferUpdater.UpdateSubMeshConstant(SubMeshConstantBuffer, true) : ConstantBufferUpdater.UpdateSubMeshConstant(SubMeshConstantBuffer, false);

        overrideMaterial[materialIndex] != nullptr ?
            UpdateMaterial(overrideMaterial[materialIndex]->GetMaterialInfo()) : UpdateMaterial(materials[materialIndex]->Material->GetMaterialInfo());

        if (renderData->IndexBuffer)
        {
            // index draw
            uint64 startIndex = renderData->MaterialSubsets[subMeshIndex].IndexStart;
            uint64 indexCount = renderData->MaterialSubsets[subMeshIndex].IndexCount;
            Graphics->DeviceContext->DrawIndexed(indexCount, startIndex, 0);
        }
    }
}

void FRenderer::RenderTexturePrimitive(
    ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices, ID3D11ShaderResourceView* _TextureSRV,
    ID3D11SamplerState* _SamplerState
) const
{
    if (!_TextureSRV || !_SamplerState)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "SRV, Sampler Error");
    }
    if (numIndices <= 0)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "numIndices Error");
    }
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &TextureStride, &offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &_TextureSRV);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &_SamplerState);
    Graphics->DeviceContext->DrawIndexed(numIndices, 0, 0);
}

void FRenderer::RenderTextPrimitive(
    ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11ShaderResourceView* _TextureSRV, ID3D11SamplerState* _SamplerState
) const
{
    if (!_TextureSRV || !_SamplerState)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "SRV, Sampler Error");
    }
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &TextureStride, &offset);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &_TextureSRV);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &_SamplerState);
    Graphics->DeviceContext->Draw(numVertices, 0);
}

void FRenderer::RenderTexturedModelPrimitive(
    ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices, ID3D11ShaderResourceView* InTextureSRV,
    ID3D11SamplerState* InSamplerState
) const
{
    if (!InTextureSRV || !InSamplerState)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "SRV, Sampler Error");
    }
    if (numIndices <= 0)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "numIndices Error");
    }
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &Stride, &offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    //Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &InTextureSRV);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &InSamplerState);

    Graphics->DeviceContext->DrawIndexed(numIndices, 0, 0);
}

void FRenderer::RenderStaticMeshes(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    PrepareShader();
    for (UStaticMeshComponent* StaticMeshComp : StaticMeshObjs)
    {
        FMatrix Model = JungleMath::CreateModelMatrix(
            StaticMeshComp->GetWorldLocation(),
            StaticMeshComp->GetWorldRotation(),
            StaticMeshComp->GetWorldScale()
        );
        // 최종 MVP 행렬
        FMatrix MVP = Model * ActiveViewport->GetViewMatrix() * ActiveViewport->GetProjectionMatrix();
        // 노말 회전시 필요 행렬
        FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        FVector4 UUIDColor = StaticMeshComp->EncodeUUID() / 255.0f;
        if (World->GetSelectedActor() == StaticMeshComp->GetOwner())
        {
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, MVP,Model, NormalMatrix, UUIDColor, true);
        }
        else
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, MVP, Model, NormalMatrix, UUIDColor, false);

        if (USkySphereComponent* skysphere = Cast<USkySphereComponent>(StaticMeshComp))
        {
            ConstantBufferUpdater.UpdateTextureConstant(TextureConstantBuffer, skysphere->UOffset, skysphere->VOffset);
        }
        else
        {
            ConstantBufferUpdater.UpdateTextureConstant(TextureConstantBuffer, 0, 0);
        }

        if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_AABB))
        {
            UPrimitiveBatch::GetInstance().RenderAABB(
                StaticMeshComp->GetBoundingBox(),
                StaticMeshComp->GetWorldLocation(),
                Model
            );
        }


        if (!StaticMeshComp->GetStaticMesh()) continue;

        OBJ::FStaticMeshRenderData* renderData = StaticMeshComp->GetStaticMesh()->GetRenderData();
        if (renderData == nullptr) continue;

        RenderPrimitive(renderData, StaticMeshComp->GetStaticMesh()->GetMaterials(), StaticMeshComp->GetOverrideMaterials(), StaticMeshComp->GetselectedSubMeshIndex());
    }
}

void FRenderer::RenderGizmos(const UWorld* World, const std::shared_ptr<FEditorViewportClient>& ActiveViewport)
{
    if (!World->GetSelectedActor())
    {
        return;
    }

#pragma region GizmoDepth
    ID3D11DepthStencilState* DepthStateDisable = Graphics->DepthStateDisable;
    Graphics->DeviceContext->OMSetDepthStencilState(DepthStateDisable, 0);
#pragma endregion GizmoDepth

    //  fill solid,  Wirframe 에서도 제대로 렌더링되기 위함
    Graphics->DeviceContext->RSSetState(UEditorEngine::graphicDevice.RasterizerStateSOLID);

    for (auto GizmoComp : GizmoObjs)
    {

        if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_TRANSLATION)
            continue;
        else if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_SCALE)
            continue;
        else if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_ROTATION)
            continue;
        FMatrix Model = JungleMath::CreateModelMatrix(GizmoComp->GetWorldLocation(),
            GizmoComp->GetWorldRotation(),
            GizmoComp->GetWorldScale()
        );
        FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        FVector4 UUIDColor = GizmoComp->EncodeUUID() / 255.0f;

        FMatrix MVP = Model * ActiveViewport->GetViewMatrix() * ActiveViewport->GetProjectionMatrix();

        if (GizmoComp == World->GetPickingGizmo())
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, MVP, Model, NormalMatrix, UUIDColor, true);
        else
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, MVP, Model, NormalMatrix, UUIDColor, false);

        if (!GizmoComp->GetStaticMesh()) continue;

        OBJ::FStaticMeshRenderData* renderData = GizmoComp->GetStaticMesh()->GetRenderData();
        if (renderData == nullptr) continue;

        RenderPrimitive(renderData, GizmoComp->GetStaticMesh()->GetMaterials(), GizmoComp->GetOverrideMaterials());
    }

    Graphics->DeviceContext->RSSetState(Graphics->GetCurrentRasterizer());

#pragma region GizmoDepth
    ID3D11DepthStencilState* originalDepthState = Graphics->DepthStencilState;
    Graphics->DeviceContext->OMSetDepthStencilState(originalDepthState, 0);
#pragma endregion GizmoDepth
}

void FRenderer::RenderBillboards(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    PrepareTextureShader();
    PrepareSubUVConstant();
    for (auto BillboardComp : BillboardObjs)
    {
        ConstantBufferUpdater.UpdateSubUVConstant(SubUVConstantBuffer, BillboardComp->finalIndexU, BillboardComp->finalIndexV);

        FMatrix Model = BillboardComp->CreateBillboardMatrix();

        // 최종 MVP 행렬
        FMatrix MVP = Model * ActiveViewport->GetViewMatrix() * ActiveViewport->GetProjectionMatrix();
        FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        FVector4 UUIDColor = BillboardComp->EncodeUUID() / 255.0f;
        if (BillboardComp == World->GetPickingGizmo())
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, MVP, Model, NormalMatrix, UUIDColor, true);
        else
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, MVP, Model, NormalMatrix, UUIDColor, false);

        if (UParticleSubUVComp* SubUVParticle = Cast<UParticleSubUVComp>(BillboardComp))
        {
            
            const FQuadRenderData& QuadRenderData = UEditorEngine::resourceMgr.GetQuadRenderData();
            RenderTexturePrimitive(
                SubUVParticle->vertexSubUVBuffer, SubUVParticle->numTextVertices,
                QuadRenderData.IndexTextureBuffer, QuadRenderData.numIndices, SubUVParticle->Texture->TextureSRV, SubUVParticle->Texture->SamplerState
            );
        }
        else if (UText* Text = Cast<UText>(BillboardComp))
        {
            UEditorEngine::renderer.RenderTextPrimitive(
                Text->vertexTextBuffer, Text->numTextVertices,
                Text->Texture->TextureSRV, Text->Texture->SamplerState
            );
        }
        else
        {
            const FQuadRenderData& QuadRenderData = UEditorEngine::resourceMgr.GetQuadRenderData();
            
            RenderTexturePrimitive(
                QuadRenderData.VertexTextureBuffer, QuadRenderData.numVertices,
                QuadRenderData.IndexTextureBuffer, QuadRenderData.numIndices, BillboardComp->Texture->TextureSRV, BillboardComp->Texture->SamplerState
            );
        }
    }
    PrepareShader();
}


void FRenderer::RenderLight(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    for (auto Light : LightObjs)
    {
        FMatrix Model = JungleMath::CreateModelMatrix(Light->GetWorldLocation(), Light->GetWorldRotation(), { 1, 1, 1 });
        UPrimitiveBatch::GetInstance().AddCone(Light->GetWorldLocation(), Light->GetRadius(), 15, 140, Light->GetColor(), Model);
        UPrimitiveBatch::GetInstance().RenderOBB(Light->GetBoundingBox(), Light->GetWorldLocation(), Model);
    }
}

void FRenderer::RenderBatch(
    const FGridParameters& gridParam, ID3D11Buffer* pVertexBuffer, int boundingBoxCount, int coneCount, int coneSegmentCount, int obbCount
) const
{
    UINT stride = sizeof(FSimpleVertex);
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    UINT vertexCountPerInstance = 2;
    UINT instanceCount = gridParam.numGridLines + 3 + (boundingBoxCount * 12) + (coneCount * (2 * coneSegmentCount)) + (12 * obbCount);
    Graphics->DeviceContext->DrawInstanced(vertexCountPerInstance, instanceCount, 0, 0);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
#pragma endregion Render

void FRenderer::ChangeViewMode(EViewModeIndex evi) const
{
    switch (evi)
    {
    case EViewModeIndex::VMI_Lit:
        ConstantBufferUpdater.UpdateLitUnlitConstant(FlagBuffer, 1);
        break;
    case EViewModeIndex::VMI_Wireframe:
    case EViewModeIndex::VMI_Unlit:
        ConstantBufferUpdater.UpdateLitUnlitConstant(FlagBuffer, 0);
        break;
    }
}

void FRenderer::UpdateMaterial(const FObjMaterialInfo& MaterialInfo) const
{
    ConstantBufferUpdater.UpdateMaterialConstant(MaterialConstantBuffer, MaterialInfo);

    if (MaterialInfo.bHasTexture == true)
    {
        std::shared_ptr<FTexture> texture = UEditorEngine::resourceMgr.GetTexture(MaterialInfo.DiffuseTexturePath);
        Graphics->DeviceContext->PSSetShaderResources(0, 1, &texture->TextureSRV);
        Graphics->DeviceContext->PSSetSamplers(0, 1, &texture->SamplerState);
    }
    else
    {
        ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
        ID3D11SamplerState* nullSampler[1] = {nullptr};

        Graphics->DeviceContext->PSSetShaderResources(0, 1, nullSRV);
        Graphics->DeviceContext->PSSetSamplers(0, 1, nullSampler);
    }
}

void FRenderer::RenderGBuffer()
{
    FGraphicsDevice graphicDevice = GEngine->graphicDevice;
    SLevelEditor* LevelEditor = GEngine->GetLevelEditor();
    std::shared_ptr<UWorld> GWorld = GEngine->GetWorld();

    graphicDevice.Prepare();
    if (LevelEditor->IsMultiViewport())
    {
        std::shared_ptr<FEditorViewportClient> viewportClient = LevelEditor->GetActiveViewportClient();
        for (int i = 0; i < 4; ++i)
        {
            LevelEditor->SetViewportClient(i);
            PrepareRender();
            Render(GWorld.get(), LevelEditor->GetActiveViewportClient());
        }
        LevelEditor->SetViewportClient(viewportClient);
    }
    else
    {
        PrepareRender();
        Render(GWorld.get(), LevelEditor->GetActiveViewportClient());
    }
}

void FRenderer::RenderLightPass()
{
}

void FRenderer::RenderPostProcessPass()
{
    // Temp
    const auto& ActiveViewport = GEngine->GetLevelEditor()->GetActiveViewportClient();
    FMatrix View = ActiveViewport->GetViewMatrix();
    FMatrix Proj = ActiveViewport->GetProjectionMatrix();
    FMatrix ViewProj = View * Proj;
    FMatrix InvViewProj = FMatrix::Inverse(ViewProj); 
    ConstantBufferUpdater.UpdateFogConstant(
            FogConstantBuffer,
            InvViewProj,
            10.0f,    // 시작 높이
            -10.0f,   // 끝 높이
            1.0f      // 밀도
    );
    
    if (!PostProcessColorSRV || !SceneDepthSRV || !PostProcessSampler || !DepthSampler || !FogConstantBuffer)
        return; 

    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 1. 셰이더 바인딩
    Graphics->DeviceContext->VSSetShader(VertexTextureShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PostProcessPixelShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(TextureInputLayout);

    // 2. 풀스크린 Quad 바인딩
    const FQuadRenderData& QuadData = UEditorEngine::resourceMgr.GetQuadRenderData();
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &QuadData.VertexTextureBuffer, &TextureStride, &offset);
    Graphics->DeviceContext->IASetIndexBuffer(QuadData.IndexTextureBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 3. ShaderResourceView 바인딩 (t0: color, t1: depth)
    ID3D11ShaderResourceView* SRVs[2] = { PostProcessColorSRV, SceneDepthSRV };
    Graphics->DeviceContext->PSSetShaderResources(0, 2, SRVs);

    // 4. SamplerState 바인딩 (s0: color, s1: depth)
    ID3D11SamplerState* Samplers[2] = { PostProcessSampler, DepthSampler };
    Graphics->DeviceContext->PSSetSamplers(0, 2, Samplers);

    // 5. Fog 상수 버퍼 바인딩 (b0)
    Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &FogConstantBuffer);

    // 6. 풀스크린 Quad 드로우
    Graphics->DeviceContext->DrawIndexed(QuadData.numIndices, 0, 0);
}

void FRenderer::RenderOverlayPass()
{
    UImGuiManager* UIMgr = GEngine->GetUIManager();
    UnrealEd* UnrealEditor = GEngine->GetUnrealEditor();

    UIMgr->BeginFrame();

    UnrealEditor->Render();
    Console::GetInstance().Draw();

    UIMgr->EndFrame();
}

ID3D11ShaderResourceView* FRenderer::CreateBoundingBoxSRV(ID3D11Buffer* pBoundingBoxBuffer, UINT numBoundingBoxes)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = numBoundingBoxes;


    Graphics->Device->CreateShaderResourceView(pBoundingBoxBuffer, &srvDesc, &pBBSRV);
    return pBBSRV;

    
}

ID3D11ShaderResourceView* FRenderer::CreateOBBSRV(ID3D11Buffer* pBoundingBoxBuffer, UINT numBoundingBoxes)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = numBoundingBoxes;
    Graphics->Device->CreateShaderResourceView(pBoundingBoxBuffer, &srvDesc, &pOBBSRV);
    return pOBBSRV;
}

ID3D11ShaderResourceView* FRenderer::CreateConeSRV(ID3D11Buffer* pConeBuffer, UINT numCones)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN; 
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = numCones;


    Graphics->Device->CreateShaderResourceView(pConeBuffer, &srvDesc, &pConeSRV);
    return pConeSRV;
}

void FRenderer::UpdateBoundingBoxBuffer(ID3D11Buffer* pBoundingBoxBuffer, const TArray<FBoundingBox>& BoundingBoxes, int numBoundingBoxes) const
{
    if (!pBoundingBoxBuffer) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(pBoundingBoxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = reinterpret_cast<FBoundingBox*>(mappedResource.pData);
    for (int i = 0; i < BoundingBoxes.Num(); ++i)
    {
        pData[i] = BoundingBoxes[i];
    }
    Graphics->DeviceContext->Unmap(pBoundingBoxBuffer, 0);
}

void FRenderer::UpdateOBBBuffer(ID3D11Buffer* pBoundingBoxBuffer, const TArray<FOBB>& BoundingBoxes, int numBoundingBoxes) const
{
    if (!pBoundingBoxBuffer) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(pBoundingBoxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = reinterpret_cast<FOBB*>(mappedResource.pData);
    for (int i = 0; i < BoundingBoxes.Num(); ++i)
    {
        pData[i] = BoundingBoxes[i];
    }
    Graphics->DeviceContext->Unmap(pBoundingBoxBuffer, 0);
}

void FRenderer::UpdateConesBuffer(ID3D11Buffer* pConeBuffer, const TArray<FCone>& Cones, int numCones) const
{
    if (!pConeBuffer) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(pConeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = reinterpret_cast<FCone*>(mappedResource.pData);
    for (int i = 0; i < Cones.Num(); ++i)
    {
        pData[i] = Cones[i];
    }
    Graphics->DeviceContext->Unmap(pConeBuffer, 0);
}

void FRenderer::UpdateGridConstantBuffer(const FGridParameters& gridParams) const
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = Graphics->DeviceContext->Map(GridConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        memcpy(mappedResource.pData, &gridParams, sizeof(FGridParameters));
        Graphics->DeviceContext->Unmap(GridConstantBuffer, 0);
    }
    else
    {
        UE_LOG(LogLevel::Warning, "gridParams");
    }
}

void FRenderer::UpdateLinePrimitveCountBuffer(int numBoundingBoxes, int numCones) const
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = Graphics->DeviceContext->Map(LinePrimitiveBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = static_cast<FPrimitiveCounts*>(mappedResource.pData);
    pData->BoundingBoxCount = numBoundingBoxes;
    pData->ConeCount = numCones;
    Graphics->DeviceContext->Unmap(LinePrimitiveBuffer, 0);
}


void FRenderer::CreateDummyPostProcessResources()
{
    const int width = 512;
    const int height = 512;

    // ----------- COLOR 텍스처 생성 (연보라색) -----------
    UINT colorData[width * height];
    for (int i = 0; i < width * height; ++i)
        colorData[i] = 0xFF6060FF;

    D3D11_TEXTURE2D_DESC colorDesc = {};
    colorDesc.Width = width;
    colorDesc.Height = height;
    colorDesc.MipLevels = 1;
    colorDesc.ArraySize = 1;
    colorDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    colorDesc.SampleDesc.Count = 1;
    colorDesc.Usage = D3D11_USAGE_DEFAULT;
    colorDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA colorInitData = {};
    colorInitData.pSysMem = colorData;
    colorInitData.SysMemPitch = width * 4;

    Graphics->Device->CreateTexture2D(&colorDesc, &colorInitData, &DummyColorTexture);
    Graphics->Device->CreateShaderResourceView(DummyColorTexture, nullptr, &DummyColorSRV);


    // ----------- DEPTH 텍스처 생성 (위는 얕고 아래는 깊은 gradient) -----------
    float depthData[width * height];
    for (int y = 0; y < height; ++y)
    {
        float depth = (float)y / height; // 아래로 갈수록 깊어짐
        for (int x = 0; x < width; ++x)
            depthData[y * width + x] = depth;
    }

    D3D11_TEXTURE2D_DESC depthDesc = colorDesc;
    depthDesc.Format = DXGI_FORMAT_R32_FLOAT;

    D3D11_SUBRESOURCE_DATA depthInitData = {};
    depthInitData.pSysMem = depthData;
    depthInitData.SysMemPitch = width * sizeof(float);

    Graphics->Device->CreateTexture2D(&depthDesc, &depthInitData, &DummyDepthTexture);
    Graphics->Device->CreateShaderResourceView(DummyDepthTexture, nullptr, &DummyDepthSRV);


    // ----------- SRV 연결 (PostProcessPass에서 사용할 것들) -----------
    PostProcessColorSRV = DummyColorSRV;
    SceneDepthSRV = DummyDepthSRV;
}
