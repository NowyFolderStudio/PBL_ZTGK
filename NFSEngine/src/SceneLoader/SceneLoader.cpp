#include <json/json.hpp>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <iostream>

#include "Components/Transform.hpp"
#include "Components/ModelComponent.hpp"
#include "SceneLoader/SceneLoader.hpp"

using json = nlohmann::json;

namespace NFSEngine {

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

        auto shader = Shader::Create("BasicShader", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
        auto texture = NFSEngine::Texture::Create("assets/textures/cat.png");

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

            if (j_obj["mesh_path"] != "") {
                auto material = std::make_shared<NFSEngine::Material>();

                std::string texPath = "assets/textures/cat.png";

                if (j_obj.contains("texture_path") && j_obj["texture_path"] != "") {
                    texPath = j_obj["texture_path"];
                }
                material->AlbedoMap = NFSEngine::Texture::Create(texPath);

                go->AddComponent<ModelComponent>(shader, material);

                auto model = std::make_shared<NFSEngine::Model>(j_obj["mesh_path"]);
                go->GetComponent<ModelComponent>()->AddLOD(model, 1000);
            }

            if (j_obj.contains("box_colliders")) {
                int colliderIndex = 0;

                for (const auto& j_bc : j_obj["box_colliders"]) {
                    GameObject* targetObj = go;

                    if (colliderIndex > 0) {
                        std::string childName = name + "_Collider_" + std::to_string(colliderIndex);
                        targetObj = currentScene->CreateGameObject(childName);

                        targetObj->GetTransform()->SetParent(go->GetTransform(), false);

                        targetObj->GetTransform()->SetPosition(glm::vec3(0.0f));
                        targetObj->GetTransform()->SetRotation(glm::vec3(0.0f));
                        targetObj->GetTransform()->SetScale(glm::vec3(1.0f));
                    }

                    auto& collider = targetObj->AddComponent<BoxCollider3DComponent>();

                    auto size = j_bc["size"];
                    auto offset = j_bc["offset"];

                    collider.Size = glm::vec3(size[0], size[1], size[2]);
                    collider.Offset = glm::vec3(offset[0], offset[1], offset[2]);
                    collider.IsTrigger = j_bc["is_trigger"];

                    colliderIndex++;
                }
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