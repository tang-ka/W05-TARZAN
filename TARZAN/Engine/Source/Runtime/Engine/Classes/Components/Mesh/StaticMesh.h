#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Components/Material/Material.h"
#include "Define.h"

class UStaticMesh : public UObject
{
    DECLARE_CLASS(UStaticMesh, UObject)

public:
    UStaticMesh();
    virtual ~UStaticMesh() override;
    const TArray<FStaticMaterial*>& GetMaterials() const { return materials; }
    uint32 GetMaterialIndex(FName MaterialSlotName) const;
    void GetUsedMaterials(TArray<UMaterial*>& Out) const;
    OBJ::FStaticMeshRenderData* GetRenderData() const { return staticMeshRenderData; }

    void SetData(OBJ::FStaticMeshRenderData* renderData);

    FWString GetPathOjbectName() const
    {
        return staticMeshRenderData->PathName + staticMeshRenderData->ObjectName;
    }

    // TODO: 임시용  고정된 Assets 폴더 경로를 추적하여 상대경로를 찾는 함수
    // 나중에 바꿔야됨
    FString ConvertToRelativePathFromAssets(const FString& AbsolutePath);

private:
    OBJ::FStaticMeshRenderData* staticMeshRenderData = nullptr;
    TArray<FStaticMaterial*> materials;
};
