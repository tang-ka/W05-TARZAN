#include "StaticMesh.h"
#include "Engine/FLoaderOBJ.h"
#include "UObject/ObjectFactory.h"

UStaticMesh::UStaticMesh()
{

}

UStaticMesh::~UStaticMesh()
{
    if (staticMeshRenderData == nullptr) return;

    if (staticMeshRenderData->VertexBuffer) {
        staticMeshRenderData->VertexBuffer->Release();
        staticMeshRenderData->VertexBuffer = nullptr;
    }

    if (staticMeshRenderData->IndexBuffer) {
        staticMeshRenderData->IndexBuffer->Release();
        staticMeshRenderData->IndexBuffer = nullptr;
    }
}

uint32 UStaticMesh::GetMaterialIndex(FName MaterialSlotName) const
{
    for (uint32 materialIndex = 0; materialIndex < materials.Num(); materialIndex++) {
        if (materials[materialIndex]->MaterialSlotName == MaterialSlotName)
            return materialIndex;
    }

    return -1;
}

void UStaticMesh::GetUsedMaterials(TArray<UMaterial*>& Out) const
{
    for (const FStaticMaterial* Material : materials)
    {
        Out.Emplace(Material->Material);
    }
}

void UStaticMesh::SetData(OBJ::FStaticMeshRenderData* renderData)
{
    staticMeshRenderData = renderData;

    uint32 verticeNum = staticMeshRenderData->Vertices.Num();
    if (verticeNum <= 0) return;
    staticMeshRenderData->VertexBuffer = GetEngine()->renderer.GetResourceManager().CreateVertexBuffer(staticMeshRenderData->Vertices);

    uint32 indexNum = staticMeshRenderData->Indices.Num();
    if (indexNum > 0)
        staticMeshRenderData->IndexBuffer = GetEngine()->renderer.GetResourceManager().CreateIndexBuffer(staticMeshRenderData->Indices);

    for (int materialIndex = 0; materialIndex < staticMeshRenderData->Materials.Num(); materialIndex++) {
        FStaticMaterial* newMaterialSlot = new FStaticMaterial();
        UMaterial* newMaterial = FManagerOBJ::CreateMaterial(staticMeshRenderData->Materials[materialIndex]);

        newMaterialSlot->Material = newMaterial;
        newMaterialSlot->MaterialSlotName = staticMeshRenderData->Materials[materialIndex].MTLName;

        materials.Add(newMaterialSlot);
    }
}

FString UStaticMesh::ConvertToRelativePathFromAssets(const FString& AbsolutePath)
{
    if (AbsolutePath.IsEmpty())
    {
        return TEXT("None"); // 빈 경로는 처리 불가
    }

    // 찾을 기준 폴더 이름
    const FString AssetsFolderName = TEXT("Assets");

    // 경로에서 "Assets" 문자열을 대소문자 무시하고 *마지막* 위치부터 찾습니다.
    // 이렇게 하면 경로 중간에 다른 "Assets" 폴더가 있어도 최종 "Assets"를 기준으로 합니다.
    int32 AssetsIndex = AbsolutePath.Find(AssetsFolderName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

    if (AssetsIndex != INDEX_NONE) // "Assets" 문자열을 찾았다면
    {
        // 찾은 "Assets" 문자열 시작 위치부터 끝까지 잘라냅니다.
        FString RelativePath = AbsolutePath.RightChop(AssetsIndex);

        // (선택적) 만약 결과 경로가 구분자로 시작하면 제거 (예: "\Assets...")
        // if (!RelativePath.IsEmpty() && (RelativePath[0] == TEXT('\\') || RelativePath[0] == TEXT('/')))
        // {
        //     RelativePath = RelativePath.RightChop(1);
        // }

        return RelativePath;
    }
    else // "Assets" 문자열을 찾지 못했다면
    {
        UE_LOG(LogLevel::Warning, TEXT("Could not find 'Assets' folder in path '%s'. Cannot convert to relative path."), *AbsolutePath);
        // "Assets"를 찾지 못했으므로 상대 경로를 만들 수 없음
        assert(false); // 야매용 저장 방식임
        return TEXT("None"); // 실패 또는 알 수 없음을 나타내는 값 반환
        // 또는 원본 경로를 그대로 반환할 수도 있습니다: return AbsolutePath; (단, 혼란 유발 가능)
    }
}

