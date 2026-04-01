#include "UI/Canvas.hpp"
#include "UI/ButtonLogic.hpp"
#include <iostream>

namespace NFSEngine {

	Canvas::Canvas() {

	}

	Canvas::~Canvas() {
	}

	UIObject& Canvas::CreateUIObject() {
		auto newUIObject = std::make_unique<UIObject>();
		UIObject& uiObjectRef = *newUIObject;
		m_UIObjects.push_back(std::move(newUIObject));

		return uiObjectRef;
	}

	void Canvas::RemoveUIObject(UIObject* uiObject) {
		m_UIObjects.erase(std::remove_if(m_UIObjects.begin(), m_UIObjects.end(),
			[uiObject](const std::unique_ptr<UIObject>& obj) { return obj.get() == uiObject; }),
			m_UIObjects.end());
	}

	void Canvas::ClearUIObjects() {
		m_UIObjects.clear();
	}

	std::vector<UIObject*> Canvas::GetUIObjects() const {
		std::vector<UIObject*> uiObjectPtrs;
		for (const auto& uiObject : m_UIObjects) {
			uiObjectPtrs.push_back(uiObject.get());
		}
		return uiObjectPtrs;
	}

	void Canvas::Draw() {
		std::sort(m_UIObjects.begin(), m_UIObjects.end(),
			[](const std::unique_ptr<UIObject>& a, const std::unique_ptr<UIObject>& b) {
				return a->Transform.Position.z < b->Transform.Position.z;
			});

		for (const auto& uiObject : m_UIObjects) {
			if (uiObject->HasComponent<ImageComponent>()) {
				ImageComponent* image = uiObject->GetComponent<ImageComponent>();
				UIRenderer::DrawQuad(uiObject->Transform, *image);
			}

			if (uiObject->HasComponent<TextComponent>()) {
				UIRenderer::DrawText(uiObject->Transform, *uiObject->GetComponent<TextComponent>());
			}
		}
	}

	void Canvas::Update() {
		for (const auto& uiObject : m_UIObjects) {
			if (uiObject->HasComponent<ButtonLogic>()) {
				uiObject->GetComponent<ButtonLogic>()->Update();
			}
		}
	}
}