#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/Scene.hpp"

namespace NFSEngine {

    class BoxColliderLoader : public IComponentLoader {
    public:
        void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
            if (!j_obj.contains("box_colliders")) { }

            int colliderIndex = 0;

            for (const auto& j_bc : j_obj["box_colliders"]) {
                GameObject* currentTarget = targetObj;

                if (colliderIndex > 0) {
                    std::string childName = targetObj->name + "_Collider_" + std::to_string(colliderIndex);
                    currentTarget = currentScene->CreateGameObject(childName);

                    currentTarget->GetTransform()->SetParent(targetObj->GetTransform(), false);

                    currentTarget->GetTransform()->SetPosition(glm::vec3(0.0f));
                    currentTarget->GetTransform()->SetRotation(glm::vec3(0.0f));
                    currentTarget->GetTransform()->SetScale(glm::vec3(1.0f));
                }

                auto& collider = currentTarget->AddComponent<BoxCollider3DComponent>();
                auto size = j_bc["size"];
                auto offset = j_bc["offset"];

                collider.Size = glm::vec3(size[0], size[1], size[2]);
                collider.Offset = glm::vec3(offset[0], offset[1], offset[2]);
                collider.IsTrigger = false;

                colliderIndex++;
            }
        }
    };

} // namespace NFSEngine