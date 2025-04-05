#include "UnrealEd/SceneMgr.h"
#include "JSON/json.hpp"
#include "UObject/Object.h"
#include "Components/SphereComp.h"
#include "Components/CubeComp.h"
#include "BaseGizmos/GizmoArrowComponent.h"
#include "UObject/ObjectFactory.h"
#include <fstream>

#include "EditorViewportClient.h"
#include "Components/UBillboardComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkySphereComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/Casts.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Engine/FLoaderOBJ.h"
#include "LevelEditor/SLevelEditor.h"
using json = nlohmann::json;

/**
 * JSON 객체로부터 FSceneData 구조체를 파싱하여 채웁니다.
 * @param j 파싱할 nlohmann::json 객체.
 * @param OutSceneData 파싱된 데이터를 저장할 FSceneData 참조.
 * @return 파싱 성공 여부 (간단한 버전에서는 bool 반환, 상세 오류 처리는 예외나 로그 사용).
 */

bool FSceneMgr::ParseSceneData(const json& j , FSceneData& OutSceneData)
{
try // JSON 파싱 및 데이터 접근 시 예외 발생 가능성 처리
    {
        // 기존 데이터 클리어
        OutSceneData.Actors.Empty();

        // Version 파싱
        if (j.contains("Version") && j["Version"].is_number_integer()) {
            OutSceneData.Version = j["Version"].get<int32>();
        } else {
            // 필수 필드가 없으면 오류 처리 또는 기본값 사용
            UE_LOG(LogLevel::Warning, TEXT("JSON does not contain a valid 'Version' field."));
            // return false; // 또는 기본값 0 등으로 진행
            OutSceneData.Version = 0;
        }

        // NextUUID 파싱 (선택적 필드 처리 예시)
        OutSceneData.NextUUID = j.value("NextUUID", 0); // 없으면 기본값 0 사용

        // --- Actors 배열 파싱 ---
        if (j.contains("Actors") && j["Actors"].is_array())
        {
            const json& actorsArrayJson = j["Actors"];
            OutSceneData.Actors.Reserve(actorsArrayJson.size()); // 미리 메모리 할당

            for (const auto& actorJson : actorsArrayJson) // 각 액터 JSON 객체 순회
            {
                if (!actorJson.is_object()) {
                    UE_LOG(LogLevel::Warning, TEXT("Item in 'Actors' array is not a valid JSON object. Skipping."));
                    continue; // 유효하지 않은 항목 건너뛰기
                }

                FActorSaveData actorData; // 현재 액터 데이터 생성

                // 액터 기본 정보 파싱 (문자열 변환 포함)
                if (actorJson.contains("ActorID") && actorJson["ActorID"].is_string()) {
                    actorData.ActorID = FString((actorJson["ActorID"].get<std::string>().c_str()));
                } else { /* 오류 처리 또는 기본값 */ }

                if (actorJson.contains("ActorClass") && actorJson["ActorClass"].is_string()) {
                    actorData.ActorClass = FString((actorJson["ActorClass"].get<std::string>().c_str()));
                } else { /* 필수 필드 오류 처리 */ UE_LOG(LogLevel::Warning,  TEXT("Actor JSON missing 'ActorClass'.")); return false; }

                // ActorLabel은 선택적일 수 있음
                actorData.ActorLabel = FString((actorJson.value("ActorLabel", "").c_str())); // 없으면 빈 문자열

                if (actorJson.contains("RootComponentID") && actorJson["RootComponentID"].is_string()) {
                     actorData.RootComponentID = FString((actorJson["RootComponentID"].get<std::string>().c_str()));
                } else { /* 루트 컴포넌트가 없는 경우 ""가 될 수 있으므로 오류는 아닐 수 있음 */ actorData.RootComponentID = TEXT("");}


                // --- Components 배열 파싱 ---
                if (actorJson.contains("Components") && actorJson["Components"].is_array())
                {
                    const json& componentsArrayJson = actorJson["Components"];
                    actorData.Components.Reserve(componentsArrayJson.size());

                    for (const auto& componentJson : componentsArrayJson) // 각 컴포넌트 JSON 객체 순회
                    {
                         if (!componentJson.is_object()) {
                            UE_LOG(LogLevel::Warning, TEXT("Item in 'Components' array is not a valid JSON object. Skipping."));
                            continue;
                         }

                         FComponentSaveData componentData; // 현재 컴포넌트 데이터 생성

                         // 컴포넌트 기본 정보 파싱
                        if (componentJson.contains("ComponentID") && componentJson["ComponentID"].is_string()) {
                            componentData.ComponentID = FString((componentJson["ComponentID"].get<std::string>().c_str()));
                        } else { /* 오류 처리 */ }

                        if (componentJson.contains("ComponentClass") && componentJson["ComponentClass"].is_string()) {
                            componentData.ComponentClass = FString((componentJson["ComponentClass"].get<std::string>().c_str()));
                        } else { /* 필수 필드 오류 처리 */ UE_LOG(LogLevel::Warning, TEXT("Component JSON missing 'ComponentClass'.")); return false; }

                        // --- Properties 객체 파싱 ---
                        if (componentJson.contains("Properties") && componentJson["Properties"].is_object())
                        {
                            const json& propertiesJson = componentJson["Properties"];
                            // Properties는 TMap이므로 Reserve 불필요

                            // JSON 객체의 모든 키-값 쌍 순회
                            for (auto it = propertiesJson.begin(); it != propertiesJson.end(); ++it)
                            {
                                // it.key()는 속성 이름(std::string), it.value()는 속성 값(json 문자열)
                                if (it.value().is_string()) // 값이 문자열인지 확인
                                {
                                    FString Key = FString((it.key().c_str()));
                                    FString Value = FString((it.value().get<std::string>().c_str()));
                                    componentData.Properties.Add(Key, Value); // TMap에 추가
                                }
                                else {
                                    // 값이 문자열이 아닌 경우 (예상치 못한 데이터) 경고
                                     UE_LOG(LogLevel::Warning,  TEXT("Property '%s' in component '%s' has non-string value. Skipping."),
                                         *FString((it.key().c_str())), *componentData.ComponentID);
                                }
                            }
                        } // Properties 파싱 끝

                        actorData.Components.Add(componentData); // 완성된 컴포넌트 데이터를 액터에 추가
                    } // 컴포넌트 루프 끝
                } // Components 배열 파싱 끝

                OutSceneData.Actors.Add(actorData); // 완성된 액터 데이터를 씬 데이터에 추가
            } // 액터 루프 끝
        } // Actors 배열 파싱 끝
        else {
             UE_LOG(LogLevel::Warning, TEXT("JSON does not contain a valid 'Actors' array. Scene might be empty."));
             // 액터가 없는 빈 씬일 수 있으므로 오류는 아님
        }

        return true; // 파싱 성공
    }
    catch (const json::parse_error& e)
    {
        // JSON 파싱 자체에서 오류 발생 시
        UE_LOG(LogLevel::Warning, TEXT("JSON parse error: %hs at byte %d"), e.what(), e.byte);
        return false;
    }
    catch (const json::type_error& e)
    {
        // JSON 타입 불일치 (예: 배열을 기대했는데 객체가 옴)
        UE_LOG(LogLevel::Warning, TEXT("JSON type error: %hs"), e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        // 기타 예외 처리
        UE_LOG(LogLevel::Warning, TEXT("An unexpected error occurred during JSON parsing: %hs"), e.what());
        return false;
    }
    catch (...)
    {
        // 알 수 없는 예외
        UE_LOG(LogLevel::Warning, TEXT("An unknown error occurred during JSON parsing."));
        return false;
    }
}


void FSceneMgr::LoadSceneFromFile(const FString& filename, UWorld& world)
{
    std::string NewFileName = *filename;
    std::ifstream inFile(NewFileName);
    if (!inFile) {
        UE_LOG(LogLevel::Error, "Failed to open file for reading: %s", *filename);
        return ;
    }

    json j;
    try {
        inFile >> j; // JSON 파일 읽기
    }
    catch (const std::exception& e) {
        UE_LOG(LogLevel::Error, "Error parsing JSON: %s", e.what());
        return;
    }

    inFile.close();

    FSceneData SceneDate;
    bool Result = ParseSceneData(j,SceneDate);
    
    int a = 0;

    

    return ;
}

std::string FSceneMgr::SerializeSceneData(const FSceneData& sceneData)
{
    json j; // 최종 JSON 객체

    // Version과 NextUUID 저장
    j["Version"] = sceneData.Version;
    // NextUUID는 이제 씬 에셋 ID 관리 등에 사용될 수 있으나,
    // 액터/컴포넌트 고유 ID는 각 데이터 내에 있으므로 그 자체의 의미는 줄어들 수 있음.
    j["NextUUID"] = sceneData.NextUUID; // 필요하다면 유지

    // --- 새로운 구조 기반 직렬화 ---

    json actorsArrayJson = json::array(); // 액터 목록을 담을 JSON 배열

    // FSceneData의 Actors 배열 순회
    for (const FActorSaveData& actorData : sceneData.Actors)
    {
        json actorJson = json::object(); // 개별 액터 정보를 담을 JSON 객체

        // 액터 기본 정보 저장 (FString -> std::string 변환 필요 시 TCHAR_TO_UTF8 사용)
        actorJson["ActorID"] = (*actorData.ActorID);
        actorJson["ActorClass"] = (*actorData.ActorClass);
        actorJson["ActorLabel"] = (*actorData.ActorLabel);
        actorJson["RootComponentID"] = (*actorData.RootComponentID);

        json componentsArrayJson = json::array(); // 컴포넌트 목록을 담을 JSON 배열

        // 액터의 Components 배열 순회
        for (const FComponentSaveData& componentData : actorData.Components)
        {
            json componentJson = json::object(); // 개별 컴포넌트 정보를 담을 JSON 객체

            // 컴포넌트 기본 정보 저장
            componentJson["ComponentID"] = (*componentData.ComponentID);
            componentJson["ComponentClass"] = (*componentData.ComponentClass);

            // Properties 맵 저장 (TMap<FString, FString> -> JSON object)
            json propertiesJson = json::object();
            for (const auto& Pair : componentData.Properties) // TMap 순회
            {
                const FString& Key = Pair.Key;
                const FString& Value = Pair.Value;
                // JSON 키와 값 모두 문자열로 저장
                propertiesJson[(*Key)] = (*Value);
            }
            componentJson["Properties"] = propertiesJson; // 프로퍼티 객체를 컴포넌트 객체에 추가

            componentsArrayJson.push_back(componentJson); // 완성된 컴포넌트 JSON을 배열에 추가
        }

        actorJson["Components"] = componentsArrayJson; // 컴포넌트 배열을 액터 객체에 추가
        actorsArrayJson.push_back(actorJson); // 완성된 액터 JSON을 배열에 추가
    }

    j["Actors"] = actorsArrayJson; // 액터 배열을 최종 JSON 객체에 추가
    
    
    return j.dump(4); // 4는 들여쓰기 수준

}

bool FSceneMgr::SaveSceneToFile(const FString& filename, const UWorld& world)
{
    FSceneData sceneData  = CreateSceneData(world);
    
    std::ofstream outFile(*filename);
    if (!outFile) {
        FString errorMessage = "Failed to open file for writing: ";
        MessageBoxA(NULL, *errorMessage, "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    std::string jsonData = SerializeSceneData(sceneData);
    outFile << jsonData;
    outFile.close();

    return true;
}

FSceneData FSceneMgr::CreateSceneData(const UWorld& world)
{
    FSceneData sceneData;
    sceneData.Version = 1;

    const TSet<AActor*>& Actors =  world.GetActors();

    sceneData.Actors.Reserve(Actors.Num());

    for (const auto& Actor : Actors)
    {
        FActorSaveData actorData;

        actorData.ActorID = Actor->GetName();
        actorData.ActorClass = Actor->GetClass()->GetName();
        actorData.ActorLabel = Actor->GetActorLabel();

        USceneComponent* RootComp = Actor->GetRootComponent();
        actorData.RootComponentID = (RootComp != nullptr) ? RootComp->GetName() : TEXT(""); // 루트 없으면 빈 문자열
        
        for (const auto& Component : Actor->GetComponents())
        {
            FComponentSaveData componentData;
            componentData.ComponentID = Component->GetName();
            componentData.ComponentClass = Component->GetClass()->GetName();
            
            //TMap<FString, FString> InProperties;
            Component->GetProperties(componentData.Properties);
            
            // 컴포넌트의 속성들을 JSON으로 변환하여 저장
            // for (const auto& Property : InProperties)
            // {
            //    FString Value = Property.Value;
            //     
            //     componentData.Properties[Property.Key] = Value;
            // }

            actorData.Components.Add(componentData);
        }
        sceneData.Actors.Add(actorData);
    }
    return sceneData;
}

