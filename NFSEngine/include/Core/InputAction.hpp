#pragma once

#include "Core/KeyCodes.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

namespace NFSEngine {

    class InputAction {
    public:
        enum class Type { Button, Axis1D, Axis2D };

        InputAction() = default;
        InputAction(const std::string& name, Type type);

        InputAction& BindKey(KeyCode key);
        InputAction& BindMouseButton(MouseCode button);
        InputAction& BindControllerButton(ControllerButton button, int controllerId = 0);
        InputAction& BindControllerAxis(ControllerAxis axis, int controllerId = 0, float scale = 1.0f, float deadZone = 0.15f);
        InputAction& BindAxis1DKeys(KeyCode positive, KeyCode negative);
        InputAction& BindMousePosition();

        void Update();

        bool IsPressed() const { return m_IsPressed; }
        bool IsDown() const { return m_IsPressed && !m_WasPressed; }
        bool IsUp() const { return !m_IsPressed && m_WasPressed; }

        float GetFloat() const { return m_Value; }
        std::pair<float, float> GetVec2() const { return m_Vec2Value; }

        const std::string& GetName() const { return m_Name; }
        Type GetType() const { return m_Type; }

    private:
        struct Binding {
            enum class Source {
                Key,
                MouseButton,
                ControllerButton,
                ControllerAxis,
                Axis1DKeys,
                MousePosition
            };
            Source source;
            int code = 0;
            int controllerId = 0;
            float scale = 1.0f;
            float deadZone = 0.15f;
            int positiveCode = -1;
            int negativeCode = -1;
        };

        std::string m_Name;
        Type m_Type = Type::Button;
        std::vector<Binding> m_Bindings;

        bool m_IsPressed = false;
        bool m_WasPressed = false;
        float m_Value = 0.0f;
        std::pair<float, float> m_Vec2Value = { 0.0f, 0.0f };
    };

    class InputActionManager {
    public:
        static InputActionManager& Get();

        InputAction& Create(const std::string& name, InputAction::Type type);
        InputAction* Find(const std::string& name);
        InputAction& Get(const std::string& name);

        void UpdateAll();

        static bool IsPressed(const std::string& name);
        static bool IsDown(const std::string& name);
        static bool IsUp(const std::string& name);
        static float GetFloat(const std::string& name);
        static std::pair<float, float> GetVec2(const std::string& name);

    private:
        InputActionManager() = default;
        InputActionManager(const InputActionManager&) = delete;
        InputActionManager& operator=(const InputActionManager&) = delete;

        std::unordered_map<std::string, InputAction> m_Actions;
    };

} // namespace NFSEngine
