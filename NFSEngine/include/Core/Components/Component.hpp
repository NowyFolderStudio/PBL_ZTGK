#pragma once

#include "Core/DeltaTime.hpp"


namespace NFSEngine {

    class GameObject;

    class Component {
        public:
            virtual ~Component() = default;

            // Use for initialization calls within an object
            virtual void OnAwake() {}
            // Use for initialization calls that may depend on other objects
            virtual void OnStart() {}
            virtual void OnUpdate(DeltaTime deltaTime) {}
            virtual void OnRender() {}
            
        protected:
            Component(GameObject* owner) : p_Owner(owner) {}
            GameObject* p_Owner = nullptr;
    };

}
