#pragma once
#include "Define.h"
#include "Container/Map.h"
#include "Engine/World.h"
#include "JSON/json.hpp"
#include "Math/Quat.h"

using json = nlohmann::json;

// 필요한 구조체 전방 선언 가능
struct FComponentSaveData;

// 액터 하나의 저장 정보를 담는 구조체
struct FActorSaveData {
    FString ActorID;       // 액터의 고유 ID FName
    FString ActorClass;    // 액터의 클래스 이름 (예: "AStaticMeshActor", "APointLight")
    FString ActorLabel;    // 에디터에서 보이는 이름 (선택적)
    // FTransform ActorTransform; // 액터 자체의 트랜스폼 (보통 루트 컴포넌트가 결정) - 필요 여부 검토

    FString RootComponentID; // 이 액터의 루트 컴포넌트 ID (아래 Components 리스트 내 ID 참조)
    TArray<FComponentSaveData> Components; // 이 액터가 소유한 컴포넌트 목록
};

// 컴포넌트 하나의 저장 정보를 담는 구조체
struct FComponentSaveData {
    FString ComponentID;   // 컴포넌트의 고유 ID (액터 내에서 유일해야 함, FName) 
    FString ComponentClass; // 컴포넌트 클래스 이름 (예: "UStaticMeshComponent", "UPointLightComponent")
    

    TMap<FString, FString> Properties; 
};


class UObject;
struct FSceneData {
    int32 Version = 0;
    int32 NextUUID = 0;
    //TMap<int32, UObject*> Primitives;
    
    TArray<FActorSaveData> Actors; // 씬에 있는 모든 액터 정보
    //TMap<int32, UObject*> Cameras;
};

class FSceneMgr
{
public:
    static bool ParseSceneData(const json& jsonStr , FSceneData& OutSceneData);
    static void LoadSceneFromFile(const FString& filename, UWorld& world);
    static bool LoadSceneFromData(const FSceneData& sceneData, UWorld* targetWorld);
    static std::string SerializeSceneData(const FSceneData& sceneData);
    static bool SaveSceneToFile(const FString& filename, const UWorld& world);
    static FSceneData CreateSceneData(const UWorld& world);
};

