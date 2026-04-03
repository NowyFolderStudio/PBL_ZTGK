#pragma once

#include <string>

namespace NFSEngine {

    class Layer {
    public:
        Layer(const std::string& debugName = "Layer");
        virtual ~Layer() = default;
        
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        // TODO: Add Timestep/DeltaTime to function below
        virtual void OnUpdate() {}
        // TOOD: Add virtual OnEvent(Event& event) method to handle events, after implementing Event class
        const std::string& GetName() const { return m_DebugName; }
    protected:
        std::string m_DebugName;
    };

}