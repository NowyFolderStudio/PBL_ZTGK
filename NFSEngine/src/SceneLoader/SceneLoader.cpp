#include <json/json.hpp>
#include <memory>
#include <unordered_map>
#include <fstream>

#include "Components/Transform.hpp"
#include "SceneLoader/SceneLoader.hpp"

#include "SceneLoader/CylinderColliderLoader.hpp"
#include "SceneLoader/ModelComponentLoader.hpp"
#include "SceneLoader/BoxColliderLoader.hpp"

using json = nlohmann::json;

namespace NFSEngine {

    void SceneLoader::RegisterLoader(std::unique_ptr<IComponentLoader> loader) { m_Loaders.push_back(std::move(loader)); }

    void SceneLoader::InitDefaultLoaders() {
        if (m_Loaders.empty()) {
            RegisterLoader(std::make_unique<ModelComponentLoader>());
            RegisterLoader(std::make_unique<BoxColliderLoader>());
            RegisterLoader(std::make_unique<CylinderColliderLoader>());
        }
    }

    float SceneLoader::GetProgress() const {
        if (m_CurrentState == LoadingState::Idle || m_CurrentState == LoadingState::ParsingJSON) {
            return 0.0f;
        }

        if (m_TotalObjectsToLoad == 0) {
            return 0.0f;
        }

        float processedObjects = static_cast<float>(m_TotalObjectsToLoad - m_ObjectsToLoadQueue.size());

        return processedObjects / static_cast<float>(m_TotalObjectsToLoad);
    }

    void SceneLoader::LoadScene(Scene* currentScene, const std::string& filepath) {

        NFS_CORE_INFO("Loading scene from file: {0}", filepath);

        std::ifstream file(filepath);

        if (!file.is_open()) {

            NFS_CORE_ERROR("Can't open file: {0}", filepath);

            return;
        }

        json j;

        try {

            file >> j;

        } catch (json::parse_error& e) {

            NFS_CORE_ERROR("JSON parse error with file {0}: {1}", filepath, e.what());

            return;
        }

        if (!j.contains("game_objects")) {

            NFS_CORE_ERROR("JSON file don't have 'game_objects' key.");

            return;
        }

        NFS_CORE_INFO("Found {0} GameObjects.", j["game_objects"].size());

        std::unordered_map<int, GameObject*> instanceMap;

        for (const auto& j_obj : j["game_objects"]) {

            int id = j_obj["id"];

            std::string name = j_obj["name"];

            auto* go = currentScene->CreateGameObject(name);

            Transform* t = go->GetTransform();

            auto pos = j_obj["transform"]["position"];

            auto rot = j_obj["transform"]["rotation"];

            auto scl = j_obj["transform"]["scale"];

            t->SetPosition(glm::vec3(pos[0], pos[1], pos[2]));

            t->SetRotation(glm::vec3(rot[0], rot[1], rot[2]));

            t->SetScale(glm::vec3(scl[0], scl[1], scl[2]));

            for (auto& loader : m_Loaders) {

                loader->Load(j_obj, go, currentScene);
            }

            instanceMap[id] = go;
        }

        for (const auto& j_obj : j["game_objects"]) {

            int id = j_obj["id"];

            int parent_id = j_obj["parent_id"];

            if (parent_id != 0) {

                if (instanceMap.find(parent_id) != instanceMap.end()) {

                    Transform* childTransform = instanceMap[id]->GetTransform();

                    Transform* parentTransform = instanceMap[parent_id]->GetTransform();

                    childTransform->SetParent(parentTransform, false);

                } else {

                    NFS_CORE_WARN("GameObject ID={0} is trying to reference non-existing parent ID={1}", id, parent_id);
                }
            }
        }

        NFS_CORE_INFO("Loading {0} success!", filepath);
    }

    void SceneLoader::LoadSceneAsync(Scene* currentScene, const std::string& filepath) {
        m_TargetScene = currentScene;
        m_CurrentState = LoadingState::ParsingJSON;
        m_InstanceMap.clear();

        NFS_CORE_INFO("Loading scene from file (Async): {0}", filepath);

        // Wczytywanie pliku tekstowego na osobnym wątku (Brak OpenGL, czyste C++)
        m_JsonFuture = std::async(std::launch::async, [filepath]() {
            std::ifstream file(filepath);
            nlohmann::json j;

            if (!file.is_open()) {
                NFS_CORE_ERROR("Can't open file: {0}", filepath);
                return j; // Zwróci pusty JSON
            }

            try {
                file >> j;
            } catch (json::parse_error& e) {
                NFS_CORE_ERROR("JSON parse error with file {0}: {1}", filepath, e.what());
            }

            return j;
        });
    }

    void SceneLoader::ProcessLoading() {
        if (m_CurrentState == LoadingState::Idle) return;

        if (m_CurrentState == LoadingState::ParsingJSON) {
            // Sprawdzamy, czy wątek poboczny skończył czytać plik
            if (m_JsonFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                m_LoadedJsonDocument = m_JsonFuture.get();

                // Zabezpieczenie przed pustym lub błędnym JSONem
                if (m_LoadedJsonDocument.empty() || !m_LoadedJsonDocument.contains("game_objects")) {
                    NFS_CORE_ERROR("JSON file don't have 'game_objects' key or file read failed.");
                    m_CurrentState = LoadingState::Idle; // Błąd ładowania, przerywamy
                    return;
                }

                NFS_CORE_INFO("Found {0} GameObjects.", m_LoadedJsonDocument["game_objects"].size());

                // Wrzucamy wszystko do kolejki
                for (const auto& j_obj : m_LoadedJsonDocument["game_objects"]) {
                    m_ObjectsToLoadQueue.push(j_obj);
                }

                m_TotalObjectsToLoad = m_ObjectsToLoadQueue.size();
                m_CurrentState = LoadingState::InstantiatingObjects;
            }
        }

        if (m_CurrentState == LoadingState::InstantiatingObjects) {
            // TIMER: Ograniczamy czas pracy, żeby nie zamrozić klatki!
            int objectsProcessedThisFrame = 0;
            const int maxObjectsPerFrame = 5; // <-- DOSTOSUJ TĘ WARTOŚĆ (zależy od wydajności i wielkości modeli)

            while (!m_ObjectsToLoadQueue.empty() && objectsProcessedThisFrame < maxObjectsPerFrame) {
                auto j_obj = m_ObjectsToLoadQueue.front();
                m_ObjectsToLoadQueue.pop();

                int id = j_obj["id"];
                std::string name = j_obj["name"];

                auto* go = m_TargetScene->CreateGameObject(name);
                Transform* t = go->GetTransform();

                // Ustawianie transformacji
                auto pos = j_obj["transform"]["position"];
                auto rot = j_obj["transform"]["rotation"];
                auto scl = j_obj["transform"]["scale"];

                t->SetPosition(glm::vec3(pos[0], pos[1], pos[2]));
                t->SetRotation(glm::vec3(rot[0], rot[1], rot[2]));
                t->SetScale(glm::vec3(scl[0], scl[1], scl[2]));

                for (auto& loader : m_Loaders) {
                    loader->Load(j_obj, go, m_TargetScene);
                }

                m_InstanceMap[id] = go;
                objectsProcessedThisFrame++;
            }

            if (m_ObjectsToLoadQueue.empty()) {
                m_CurrentState = LoadingState::Finalizing;
            }
        }

        if (m_CurrentState == LoadingState::Finalizing) {
            for (const auto& j_obj : m_LoadedJsonDocument["game_objects"]) {
                int id = j_obj["id"];
                int parent_id = j_obj["parent_id"];

                if (parent_id != 0) {
                    if (m_InstanceMap.find(parent_id) != m_InstanceMap.end()) {
                        Transform* childTransform = m_InstanceMap[id]->GetTransform();
                        Transform* parentTransform = m_InstanceMap[parent_id]->GetTransform();
                        childTransform->SetParent(parentTransform, false);
                    } else {
                        NFS_CORE_WARN("GameObject ID={0} is trying to reference non-existing parent ID={1}", id, parent_id);
                    }
                }
            }

            NFS_CORE_INFO("Loading success!");

            // Zwalniamy pamięć z JSON-a, ponieważ nie będzie już potrzebny
            m_LoadedJsonDocument.clear();
            m_CurrentState = LoadingState::Idle; // Koniec ładowania!
        }
    }

} // namespace NFSEngine