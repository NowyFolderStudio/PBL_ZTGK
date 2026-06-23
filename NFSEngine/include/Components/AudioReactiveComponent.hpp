#pragma once

#include "Components/ModelComponent.hpp"
#include "Core/GameObject.hpp"
#include "Core/AudioManager.hpp"
#include "Core/Log.hpp"
#include "Renderer/Material.hpp"
#include <string>
#include <vector>

namespace NFSEngine {

    class AudioReactiveComponent : public Component {
    public:
        std::string TargetTrackName = "HitHat";

        AudioReactiveComponent(GameObject* owner)
            : Component(owner) { }

        std::string GetName() const override { return "AudioReactiveComponent"; }

    protected:
        void OnAwake() override {
            auto* model = GetOwner()->GetComponent<ModelComponent>();
            if (!model) return;
            for (auto material : model->GetMaterials()) {
                m_AnimatedMaterials.push_back(material.get());
            }
        }
        void OnUpdate(DeltaTime deltaTime) override {
            if (m_AnimatedMaterials.empty() || TargetTrackName.empty()) return;

            float distToEvent = AudioManager::GetDistanceToEventForTrack(TargetTrackName);

            for (const auto& mat : m_AnimatedMaterials) {
                mat->SetFloat("u_DistToEvent", distToEvent);
            }
        }

    private:
        std::vector<Material*> m_AnimatedMaterials;
    };

} // namespace NFSEngine