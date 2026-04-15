#pragma once

#include "Core/DeltaTime.hpp"
#include <string>
#include "Events/Event.hpp"

namespace NFSEngine {

    class Layer {
    public:
        Layer(const std::string& debugName = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() { }
        virtual void OnDetach() { }
        virtual void OnUpdate(DeltaTime deltaTime) { }
        virtual void OnRender() { }
        virtual void OnImGuiRender() { }
        virtual void OnEvent(Event& e) { }
        const std::string& GetName() const { return m_DebugName; }

    protected:
        std::string m_DebugName;
    };

} // namespace NFSEngine