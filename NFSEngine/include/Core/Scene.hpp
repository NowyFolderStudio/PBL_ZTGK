#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Core/GameObject.hpp"
#include "Core/DeltaTime.hpp"

namespace NFSEngine {

    class Scene {
    public:
        Scene() = default;
        ~Scene() = default;

        GameObject* CreateGameObject(const std::string& name = "GameObject");

        void OnUpdate(DeltaTime deltaTime);
        void OnRender();

        DeltaTime GetFixedDelta() { return m_fixedDeltaTime; }
        void SetFixedStep(DeltaTime fixedDeltaTime) { m_fixedDeltaTime = fixedDeltaTime; }

    private:
        std::vector<std::unique_ptr<GameObject>> m_GameObjects;
        DeltaTime m_fixedDeltaTime = 0.01666f;
        float m_Accumulator = 0.0f;
    };

}