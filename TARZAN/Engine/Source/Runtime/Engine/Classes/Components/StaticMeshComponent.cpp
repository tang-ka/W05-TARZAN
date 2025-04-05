#include "Components/StaticMeshComponent.h"

#include "Engine/World.h"
#include "Launch/EditorEngine.h"
#include "UObject/ObjectFactory.h"
#include "UnrealEd/PrimitiveBatch.h"


UStaticMeshComponent::UStaticMeshComponent(const UStaticMeshComponent& Other)
    : UMeshComponent(Other),
      staticMesh(Other.staticMesh),
      selectedSubMeshIndex(Other.selectedSubMeshIndex)
{
}
uint32 UStaticMeshComponent::GetNumMaterials() const
{
    if (staticMesh == nullptr) return 0;

    return staticMesh->GetMaterials().Num();
}

UMaterial* UStaticMeshComponent::GetMaterial(uint32 ElementIndex) const
{
    if (staticMesh != nullptr)
    {
        if (OverrideMaterials[ElementIndex] != nullptr)
        {
            return OverrideMaterials[ElementIndex];
        }
    
        if (staticMesh->GetMaterials().IsValidIndex(ElementIndex))
        {
            return staticMesh->GetMaterials()[ElementIndex]->Material;
        }
    }
    return nullptr;
}

uint32 UStaticMeshComponent::GetMaterialIndex(FName MaterialSlotName) const
{
    if (staticMesh == nullptr) return -1;

    return staticMesh->GetMaterialIndex(MaterialSlotName);
}

TArray<FName> UStaticMeshComponent::GetMaterialSlotNames() const
{
    TArray<FName> MaterialNames;
    if (staticMesh == nullptr) return MaterialNames;

    for (const FStaticMaterial* Material : staticMesh->GetMaterials())
    {
        MaterialNames.Emplace(Material->MaterialSlotName);
    }

    return MaterialNames;
}

void UStaticMeshComponent::GetUsedMaterials(TArray<UMaterial*>& Out) const
{
    if (staticMesh == nullptr) return;
    staticMesh->GetUsedMaterials(Out);
    for (int materialIndex = 0; materialIndex < GetNumMaterials(); materialIndex++)
    {
        if (OverrideMaterials[materialIndex] != nullptr)
        {
            Out[materialIndex] = OverrideMaterials[materialIndex];
        }
    }
}

int UStaticMeshComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    if (!AABB.Intersect(rayOrigin, rayDirection, pfNearHitDistance)) return 0;
    int nIntersections = 0;
    if (staticMesh == nullptr) return 0;

    OBJ::FStaticMeshRenderData* renderData = staticMesh->GetRenderData();

    FVertexSimple* vertices = renderData->Vertices.GetData();
    int vCount = renderData->Vertices.Num();
    UINT* indices = renderData->Indices.GetData();
    int iCount = renderData->Indices.Num();

    if (!vertices) return 0;
    BYTE* pbPositions = reinterpret_cast<BYTE*>(renderData->Vertices.GetData());

    int nPrimitives = (!indices) ? (vCount / 3) : (iCount / 3);
    float fNearHitDistance = FLT_MAX;
    for (int i = 0; i < nPrimitives; i++) {
        int idx0, idx1, idx2;
        if (!indices) {
            idx0 = i * 3;
            idx1 = i * 3 + 1;
            idx2 = i * 3 + 2;
        }
        else {
            idx0 = indices[i * 3];
            idx2 = indices[i * 3 + 1];
            idx1 = indices[i * 3 + 2];
        }

        // 각 삼각형의 버텍스 위치를 FVector로 불러옵니다.
        uint32 stride = sizeof(FVertexSimple);
        FVector v0 = *reinterpret_cast<FVector*>(pbPositions + idx0 * stride);
        FVector v1 = *reinterpret_cast<FVector*>(pbPositions + idx1 * stride);
        FVector v2 = *reinterpret_cast<FVector*>(pbPositions + idx2 * stride);

        float fHitDistance;
        if (IntersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, fHitDistance)) {
            if (fHitDistance < fNearHitDistance) {
                pfNearHitDistance = fNearHitDistance = fHitDistance;
            }
            nIntersections++;
        }

    }
    return nIntersections;
}
UObject* UStaticMeshComponent::Duplicate() const
{
    UStaticMeshComponent* NewComp = FObjectFactory::ConstructObjectFrom<UStaticMeshComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void UStaticMeshComponent::DuplicateSubObjects(const UObject* Source)
{
    UMeshComponent::DuplicateSubObjects(Source);
    // staticMesh는 복사 생성자에서 복제됨
}

void UStaticMeshComponent::PostDuplicate() {}

void UStaticMeshComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);
    
    //StaticMesh 경로 저장
    UStaticMesh* CurrentMesh = GetStaticMesh(); // 예시 함수, 실제 함수 이름 사용
    if (CurrentMesh != nullptr) {

        // 1. std::wstring 경로 얻기
        std::wstring PathWString = CurrentMesh->GetPathOjbectName(); // 이 함수가 std::wstring 반환 가정

        // 2. std::wstring을 FString으로 변환
        FString PathFString(PathWString.c_str()); // c_str()로 const wchar_t* 얻어서 FString 생성
        
        OutProperties.Add(TEXT("StaticMeshPath"), PathFString);
    } else {
        OutProperties.Add(TEXT("StaticMeshPath"), TEXT("None")); // 메시 없음 명시
        
    }
    
    
}

void UStaticMeshComponent::TickComponent(float DeltaTime)
{
    //Timer += DeltaTime * 0.005f;
    //SetLocation(GetWorldLocation()+ (FVector(1.0f,1.0f, 1.0f) * sin(Timer)));
}
