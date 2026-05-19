#include "Components/DirectionalLight.hpp"

#include <imgui.h>

#include "Core/Scene.hpp"

namespace NFSEngine {

	DirectionalLight::~DirectionalLight() {
		if (GetOwner() && GetOwner()->GetScene()) {
			GetOwner()->GetScene()->UnregisterDirectionalLight(this);
		}
	}
	
	void DirectionalLight::OnAwake() {
		if (GetOwner() && GetOwner()->GetScene()) {
			GetOwner()->GetScene()->RegisterDirectionalLight(this);
		}
	}

	void DirectionalLight::OnImGuiRender() {
		ImGui::ColorEdit3("Color", glm::value_ptr(Color));
		ImGui::DragFloat3("Direction", glm::value_ptr(Direction), 0.01f);
		ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
	}
}