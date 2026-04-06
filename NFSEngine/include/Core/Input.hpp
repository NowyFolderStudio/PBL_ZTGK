#pragma once

#include "Core/KeyCodes.hpp"
#include <utility>

namespace NFSEngine {

    class Input {
    public:
        static bool IsKeyPressed(KeyCode keycode) { return instance->IsKeyPressedImpl(keycode); }
        static bool IsKeyDown(KeyCode keycode) { return instance->IsKeyDownImpl(keycode); }
        static bool IsKeyUp(KeyCode keycode) { return instance->IsKeyUpImpl(keycode); }

        static bool IsMouseButtonPressed(MouseCode button) { return instance->IsMouseButtonPressedImpl(button); }
        static bool IsMouseButtonDown(MouseCode button) { return instance->IsMouseButtonDownImpl(button); }
        static bool IsMouseButtonUp(MouseCode button) { return instance->IsMouseButtonUpImpl(button); }

        static std::pair<float, float> GetMousePos() { return instance->GetMousePosImpl(); }
        static float GetMouseX() { return instance->GetMouseXImpl(); }
        static float GetMouseY() { return instance->GetMouseYImpl(); }

        static void UpdateStates() { instance->UpdateStatesImpl(); }

        virtual ~Input() = default;

    protected:
        virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;
        virtual bool IsKeyDownImpl(KeyCode keycode) = 0;
        virtual bool IsKeyUpImpl(KeyCode keycode) = 0;

        virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
        virtual bool IsMouseButtonDownImpl(MouseCode button) = 0;
        virtual bool IsMouseButtonUpImpl(MouseCode button) = 0;

        virtual std::pair<float, float> GetMousePosImpl() = 0;
        virtual float GetMouseXImpl() = 0;
        virtual float GetMouseYImpl() = 0;

        virtual void UpdateStatesImpl() = 0;

    private:
        static Input* instance;

        friend class Application;
    };

}