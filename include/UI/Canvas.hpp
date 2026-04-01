#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "UI/UIComponents.hpp"
#include "UI/UIRenderer.hpp"
#include "UI/UIObject.hpp"

namespace NFSEngine {
	class Canvas {
	public:
		Canvas();
		~Canvas();
		
		UIObject& CreateUIObject();
		void RemoveUIObject(class UIObject* uiObject);
		void ClearUIObjects();
		
		std::vector<class UIObject*> GetUIObjects() const;

		void Draw();
		
	private:
		std::vector<std::unique_ptr<UIObject>> m_UIObjects;
	};
}