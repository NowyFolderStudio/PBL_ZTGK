#include "Components/PointLight.hpp"

#include <imgui.h>

#include "Core/Scene.hpp"

namespace NFSEngine {

    PointLight::~PointLight() {
        if (GetOwner() && GetOwner()->GetScene()) {
            GetOwner()->GetScene()->UnregisterPointLight(this);
        }
    }

    void PointLight::OnAwake() { 
        m_Transform = m_Owner->GetComponent<Transform>(); 

        if (GetOwner() && GetOwner()->GetScene()) {
            GetOwner()->GetScene()->RegisterPointLight(this);
        }
    }

    void PointLight::OnImGuiRender() {
        ImGui::ColorEdit3("Color", glm::value_ptr(Color));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);

        ImGui::Separator();
        ImGui::Text("Attenuation (Zanikanie)");

        ImGui::DragFloat("Constant", &Constant, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Linear", &Linear, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Quadratic", &Quadratic, 0.0001f, 0.0f, 1.0f);
    }
}