#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/Scene.hpp"

namespace NFSEngine {

    class CylinderColliderLoader : public IComponentLoader {
    public:
        void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
            if (!j_obj.contains("cylinder_colliders")) {
                return;
            }

            int colliderIndex = 0;

            for (const auto& j_cc : j_obj["cylinder_colliders"]) {
                GameObject* currentTarget = targetObj;

                if (colliderIndex > 0) {
                    std::string childName = targetObj->name + "_CylinderCollider_" + std::to_string(colliderIndex);
                    currentTarget = currentScene->CreateGameObject(childName);

                    currentTarget->GetTransform()->SetParent(targetObj->GetTransform(), false);

                    currentTarget->GetTransform()->SetPosition(glm::vec3(0.0f));
                    currentTarget->GetTransform()->SetRotation(glm::vec3(0.0f));
                    currentTarget->GetTransform()->SetScale(glm::vec3(1.0f));
                }

                auto& collider = currentTarget->AddComponent<CylinderCollider3DComponent>();

                collider.Radius = j_cc["radius"].get<float>();
                collider.Height = j_cc["height"].get<float>();

                auto offset = j_cc["offset"];
                collider.Offset = glm::vec3(offset[0], offset[1], offset[2]);

                if (j_cc.contains("is_trigger")) {
                    collider.IsTrigger = j_cc["is_trigger"].get<bool>();
                } else {
                    collider.IsTrigger = false;
                }

                colliderIndex++;
            }
        }
    };

} // namespace NFSEngine