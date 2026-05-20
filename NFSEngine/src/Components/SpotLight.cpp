#include "Components/SpotLight.hpp"

#include <imgui.h>

#include "Core/Scene.hpp"

namespace NFSEngine {

    SpotLight::~SpotLight() {
        if (GetOwner() && GetOwner()->GetScene()) {
            GetOwner()->GetScene()->UnregisterSpotLight(this);
        }
    }

    void SpotLight::OnImGuiRender() {
        ImGui::ColorEdit3("Color", glm::value_ptr(Color));
        ImGui::DragFloat3("Direction", glm::value_ptr(Direction), 0.01f);
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 1000.0f);

        ImGui::Separator();
        ImGui::Text("Angles (Ky w stopniach)");

        float innerAngle = glm::degrees(glm::acos(CutOff));
        float outerAngle = glm::degrees(glm::acos(OuterCutOff));

        bool anglesChanged = false;
        if (ImGui::DragFloat("Inner Angle", &innerAngle, 0.5f, 0.0f, 90.0f)) anglesChanged = true;
        if (ImGui::DragFloat("Outer Angle", &outerAngle, 0.5f, 0.0f, 90.0f)) anglesChanged = true;

        if (anglesChanged) {
            if (outerAngle < innerAngle) {
                outerAngle = innerAngle;
            }

            CutOff = glm::cos(glm::radians(innerAngle));
            OuterCutOff = glm::cos(glm::radians(outerAngle));
        }

        ImGui::Separator();
        ImGui::Text("Attenuation (Zanikanie)");
        ImGui::DragFloat("Constant", &Constant, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Linear", &Linear, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Quadratic", &Quadratic, 0.0001f, 0.0f, 1.0f);
    }

    void SpotLight::OnAwake() { 
        m_Transform = m_Owner->GetComponent<Transform>(); 

        if (GetOwner() && GetOwner()->GetScene()) {
            GetOwner()->GetScene()->RegisterSpotLight(this);
        }
    }
}