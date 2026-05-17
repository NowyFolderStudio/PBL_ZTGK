#include <json/json.hpp>
#include <memory>
#include <unordered_map>
#include <fstream>

#include "Components/Transform.hpp"
#include "SceneLoader/SceneLoader.hpp"

#include "SceneLoader/ModelComponentLoader.hpp"
#include "SceneLoader/BoxColliderLoader.hpp"

using json = nlohmann::json;

namespace NFSEngine {

    void SceneLoader::RegisterLoader(std::unique_ptr<IComponentLoader> loader) { m_Loaders.push_back(std::move(loader)); }

    void SceneLoader::InitDefaultLoaders() {
        if (m_Loaders.empty()) {
            RegisterLoader(std::make_unique<ModelComponentLoader>());
            RegisterLoader(std::make_unique<BoxColliderLoader>());
        }
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

} // namespace NFSEngine