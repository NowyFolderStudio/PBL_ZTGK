#pragma once

#include "Components/CubeMesh.hpp"
#include "Components/ModelComponent.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Frustum.hpp"

namespace NFSEngine { namespace CullingUtils {

    inline BoundingSphere GetLocalBoundingSphere(GameObject* go) {
        BoundingSphere result = {glm::vec3(0.0f), 0.0f};

        if (auto* cm = go->GetComponent<CubeMesh>()) {
            result = {glm::vec3(0.0f), 0.866f};
        }
        if (auto* mm = go->GetComponent<ModelComponent>()) {
            const auto& lods = mm->GetLODs();
            if (!lods.empty() && lods[0].ModelData) {
                result = lods[0].ModelData->GetMeshBoundingSphere();
            }
        }

        return result;
    }

    inline std::pair<glm::vec3, glm::vec3> GetLocalAABB(GameObject* go) {
        if (auto* cm = go->GetComponent<CubeMesh>()) {
            return {{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}};
        }
        if (auto* mm = go->GetComponent<ModelComponent>()) {
            const auto& lods = mm->GetLODs();
            if (!lods.empty() && lods[0].ModelData) {
                return {lods[0].ModelData->GetMeshAABBMin(), lods[0].ModelData->GetMeshAABBMax()};
            }
        }
        return {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    }

}} // namespace NFSEngine::CullingUtils
