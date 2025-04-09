#pragma once
#include "Components/MeshComponent.h"
#include "Mesh/StaticMesh.h"

class UStaticMeshComponent : public UMeshComponent
{
    DECLARE_CLASS(UStaticMeshComponent, UMeshComponent)

public:
    UStaticMeshComponent() = default;
    UStaticMeshComponent(const UStaticMeshComponent& Other);

    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;
    
    void GetProperties(TMap<FString, FString>& OutProperties) const override;
    
    void SetProperties(const TMap<FString, FString>& InProperties) override;
    
    virtual void TickComponent(float DeltaTime) override;

    PROPERTY(int, selectedSubMeshIndex);

    virtual uint32 GetNumMaterials() const override;
    virtual UMaterial* GetMaterial(uint32 ElementIndex) const override;
    virtual uint32 GetMaterialIndex(FName MaterialSlotName) const override;
    virtual TArray<FName> GetMaterialSlotNames() const override;
    virtual void GetUsedMaterials(TArray<UMaterial*>& Out) const override;

    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance) override;
    
    UStaticMesh* GetStaticMesh() const { return staticMesh; }
    void SetStaticMesh(UStaticMesh* value)
    { 
        staticMesh = value;
        OverrideMaterials.SetNum(value->GetMaterials().Num());
        AABB = FBoundingBox(staticMesh->GetRenderData()->BoundingBoxMin, staticMesh->GetRenderData()->BoundingBoxMax);
    }
    

protected:
    UStaticMesh* staticMesh = nullptr;
    int selectedSubMeshIndex = -1;
private:
    float Timer = 0.0f;
};

class UCylinderComponent : public UStaticMeshComponent
{
    DECLARE_CLASS(UCylinderComponent, UStaticMeshComponent)

public:
    UCylinderComponent();
    virtual ~UCylinderComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
};


class UConeComponent : public UStaticMeshComponent
{
    DECLARE_CLASS(UConeComponent, UStaticMeshComponent)

public:
    UConeComponent();
    virtual ~UConeComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
};

class UPlaneComponent : public UStaticMeshComponent
{
    DECLARE_CLASS(UPlaneComponent, UStaticMeshComponent)

public:
    UPlaneComponent();
    virtual ~UPlaneComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
};


class USphereComponent : public UStaticMeshComponent
{
    DECLARE_CLASS(USphereComponent, UStaticMeshComponent)

public:
    USphereComponent();
    virtual ~USphereComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
};


class UTorusComponent : public UStaticMeshComponent
{
    DECLARE_CLASS(UTorusComponent, UStaticMeshComponent)

public:
    UTorusComponent();
    virtual ~UTorusComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
};
