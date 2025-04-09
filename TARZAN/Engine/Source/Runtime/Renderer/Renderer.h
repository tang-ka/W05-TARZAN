#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#define _TCHAR_DEFINED
#include <d3d11.h>
#include "EngineBaseTypes.h"
#include "Define.h"
#include "Container/Set.h"
#include "RenderResourceManager.h"
#include "ShaderManager.h"
#include "ConstantBufferUpdater.h"
#include "Pass/RenderPass.h"

class ULightComponentBase;
class UFireballComponent;
class UWorld;
class FGraphicsDevice;
class UMaterial;
struct FStaticMaterial;
class UObject;
class FEditorViewportClient;
class UBillboardComponent;
class UStaticMeshComponent;
class UGizmoBaseComponent;
class FRenderResourceManager;

class FRenderer 
{
public:
    //FRenderer() : UIMgr(nullptr) {}
    ~FRenderer();
    
    void Render();
private:
    void RenderPass(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void RenderImGui();
private:
    float litFlag = 0;
public:
    FGraphicsDevice* Graphics;

    // Full Screen Vertex Shader
    ID3D11VertexShader* FullScreenVS = nullptr;
    ID3D11InputLayout* FullScreenInputLayout = nullptr;
    
    // GBuffer Shader
    ID3D11VertexShader* GBufferVS= nullptr;
    ID3D11PixelShader* GBufferPS = nullptr;
    ID3D11InputLayout* GBufferInputLayout = nullptr;
    
    // Lighting Shader
    ID3D11PixelShader* LightingPassPS = nullptr;

    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11PixelShader* PostProcessPixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;

    ID3D11Buffer* ConstantBuffer = nullptr;
    ID3D11Buffer* LightingBuffer = nullptr;
    ID3D11Buffer* FlagBuffer = nullptr;
    ID3D11Buffer* MaterialConstantBuffer = nullptr;
    ID3D11Buffer* SubMeshConstantBuffer = nullptr;
    ID3D11Buffer* TextureConstantBuffer = nullptr;
    ID3D11Buffer* FireballConstantBuffer = nullptr;
    ID3D11Buffer* LPLightConstantBuffer = nullptr;
    ID3D11Buffer* LPMaterialConstantBuffer = nullptr;
    ID3D11Buffer* FogConstantBuffer = nullptr;
    ID3D11Buffer* ScreenConstantBuffer = nullptr;

    FLighting lightingData;
    FFogConstants fogData;

    uint32 FullScreenStride;

    uint32 Stride;
    uint32 Stride2;

    ID3D11SamplerState* LPSamplerState;

public:
    void Initialize(FGraphicsDevice* graphics);
   
    void PrepareShader() const;
    
    //Render
    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices) const;
    void RenderPrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices) const;
    void RenderPrimitive(OBJ::FStaticMeshRenderData* renderData, TArray<FStaticMaterial*> materials, TArray<UMaterial*> overrideMaterial, int selectedSubMeshIndex) const;
   
    void RenderTexturedModelPrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices, ID3D11ShaderResourceView* InTextureSRV, ID3D11SamplerState* InSamplerState) const;
    //Release
    void Release();
    void ReleaseShader();
    void ReleaseConstantBuffer();

    void CreateShader();
    
    void ChangeViewMode(EViewModeIndex evi) const;
    
    // CreateBuffer
    void CreateConstantBuffer();

    // update
    void UpdateMaterial(const FObjMaterialInfo& MaterialInfo) const;

private:
#pragma region Render Pass
    void RenderGBuffer(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void RenderLightPass(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void RenderPostProcessPass(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void RenderOverlayPass(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
#pragma endregion

public://텍스쳐용 기능 추가
    ID3D11VertexShader* VertexTextureShader = nullptr;
    ID3D11PixelShader* PixelTextureShader = nullptr;
    ID3D11InputLayout* TextureInputLayout = nullptr;

    uint32 TextureStride;
    struct FSubUVConstant
    {
        float indexU;
        float indexV;
    };
    ID3D11Buffer* SubUVConstantBuffer = nullptr;

public:
    void PrepareLightShader() const;
    void PrepareTextureShader() const;

    void RenderTexturePrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices,
        ID3D11Buffer* pIndexBuffer, UINT numIndices,
        ID3D11ShaderResourceView* _TextureSRV,
        ID3D11SamplerState* _SamplerState) const;
    void RenderTextPrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices,
        ID3D11ShaderResourceView* _TextureSRV,
        ID3D11SamplerState* _SamplerState) const;

    void PrepareSubUVConstant() const;


public: // line shader
    void PrepareLineShader() const;
    void RenderBatch(const FGridParameters& gridParam, ID3D11Buffer* pVertexBuffer, int boundingBoxCount, int coneCount, int coneSegmentCount, int obbCount) const;
    void UpdateGridConstantBuffer(const FGridParameters& gridParams) const;
    void UpdateLinePrimitveCountBuffer(int numBoundingBoxes, int numCones) const;

    ID3D11ShaderResourceView* CreateBoundingBoxSRV(ID3D11Buffer* pBoundingBoxBuffer, UINT numBoundingBoxes);
    ID3D11ShaderResourceView* CreateOBBSRV(ID3D11Buffer* pBoundingBoxBuffer, UINT numBoundingBoxes);
    ID3D11ShaderResourceView* CreateConeSRV(ID3D11Buffer* pConeBuffer, UINT numCones);

    void UpdateBoundingBoxBuffer(ID3D11Buffer* pBoundingBoxBuffer, const TArray<FBoundingBox>& BoundingBoxes, int numBoundingBoxes) const;
    void UpdateOBBBuffer(ID3D11Buffer* pBoundingBoxBuffer, const TArray<FOBB>& BoundingBoxes, int numBoundingBoxes) const;
    void UpdateConesBuffer(ID3D11Buffer* pConeBuffer, const TArray<FCone>& Cones, int numCones) const;

    //Render Pass Demo
    void PrepareRender();
    void ClearRenderArr();
    void Render(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void RenderStaticMeshes(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void RenderGizmos(const UWorld* World, const std::shared_ptr<FEditorViewportClient>& ActiveViewport);
    void RenderLight(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void RenderBillboards(UWorld* World,std::shared_ptr<FEditorViewportClient> ActiveViewport);
private:
    // GBuffer
    TArray<UStaticMeshComponent*> StaticMeshObjs;
    TArray<UBillboardComponent*> BillboardObjs;
    // Lighting
    TArray<ULightComponentBase*> LightObjs;
    TArray<UFireballComponent*> FireballObjs;
    // Overaly
    TArray<UGizmoBaseComponent*> GizmoObjs;

public:
    ID3D11VertexShader* VertexLineShader = nullptr;
    ID3D11PixelShader* PixelLineShader = nullptr;
    ID3D11Buffer* GridConstantBuffer = nullptr;
    ID3D11Buffer* LinePrimitiveBuffer = nullptr;
    ID3D11ShaderResourceView* pBBSRV = nullptr;
    ID3D11ShaderResourceView* pConeSRV = nullptr;
    ID3D11ShaderResourceView* pOBBSRV = nullptr;

public:
    FRenderResourceManager& GetResourceManager() { return RenderResourceManager; }
    FShaderManager& GetShaderManager() { return ShaderManager; }
    FConstantBufferUpdater& GetConstantBufferUpdater() { return ConstantBufferUpdater; }
    //UnrealEd* GetUnrealEditor() { return UnrealEditor; }

private:
    FRenderResourceManager RenderResourceManager;
    FShaderManager ShaderManager;
    FConstantBufferUpdater ConstantBufferUpdater;

    //UImGuiManager* UIMgr;
    // TODO: 리펙토링 할 때 이거 EditorEngine에서 다 옮겨와야함.
    //UnrealEd* UnrealEditor;
    //FGraphicsDevice* graphicDevice;
    //FResourceMgr* reourceMgr;
    
private:
    ID3D11DeviceContext* Context;

private:
    // PostProcess용 Dummy Data (Todo : 추후 LightPass와 연결)
    ID3D11Texture2D* DummyColorTexture = nullptr;
    ID3D11ShaderResourceView* DummyColorSRV = nullptr;
    void CreateDummyPostProcessResources();
    
    ID3D11ShaderResourceView* PostProcessColorSRV = nullptr; // t0
    ID3D11ShaderResourceView* SceneDepthSRV = nullptr;       // t1

    ID3D11SamplerState* PostProcessSampler = nullptr; // s0
    ID3D11SamplerState* DepthSampler = nullptr;       // s1
};

