#include "Core/InputAction.hpp"
#include "Core/Input.hpp"
#include <algorithm>
#include <cmath>

namespace NFSEngine {

    // --- InputAction ---

    InputAction::InputAction(const std::string& name, Type type)
        : m_Name(name), m_Type(type) {
    }

    InputAction& InputAction::BindKey(KeyCode key) {
        Binding b;
        b.source = Binding::Source::Key;
        b.code = key;
        m_Bindings.push_back(b);
        return *this;
    }

    InputAction& InputAction::BindMouseButton(MouseCode button) {
        Binding b;
        b.source = Binding::Source::MouseButton;
        b.code = button;
        m_Bindings.push_back(b);
        return *this;
    }

    InputAction& InputAction::BindControllerButton(ControllerButton button, int controllerId) {
        Binding b;
        b.source = Binding::Source::ControllerButton;
        b.code = button;
        b.controllerId = controllerId;
        m_Bindings.push_back(b);
        return *this;
    }

    InputAction& InputAction::BindControllerAxis(ControllerAxis axis, int controllerId, float scale, float deadZone) {
        Binding b;
        b.source = Binding::Source::ControllerAxis;
        b.code = axis;
        b.controllerId = controllerId;
        b.scale = scale;
        b.deadZone = deadZone;
        m_Bindings.push_back(b);
        return *this;
    }

    InputAction& InputAction::BindAxis1DKeys(KeyCode positive, KeyCode negative) {
        Binding b;
        b.source = Binding::Source::Axis1DKeys;
        b.positiveCode = positive;
        b.negativeCode = negative;
        m_Bindings.push_back(b);
        return *this;
    }

    InputAction& InputAction::BindMousePosition() {
        Binding b;
        b.source = Binding::Source::MousePosition;
        m_Bindings.push_back(b);
        return *this;
    }

    void InputAction::Update() {
        m_WasPressed = m_IsPressed;
        m_IsPressed = false;
        m_Value = 0.0f;
        m_Vec2Value = { 0.0f, 0.0f };

        if (m_Type == Type::Button) {
            for (auto& b : m_Bindings) {
                bool pressed = false;
                switch (b.source) {
                case Binding::Source::Key:
                    pressed = Input::IsKeyPressed(b.code);
                    break;
                case Binding::Source::MouseButton:
                    pressed = Input::IsMouseButtonPressed(b.code);
                    break;
                case Binding::Source::ControllerButton:
                    pressed = Input::IsControllerPresent(b.controllerId) && Input::IsControllerButtonPressed(b.controllerId, b.code);
                    break;
                default:
                    break;
                }
                if (pressed) {
                    m_IsPressed = true;
                    break;
                }
            }
        }
        else if (m_Type == Type::Axis1D) {
            for (auto& b : m_Bindings) {
                switch (b.source) {
                case Binding::Source::Axis1DKeys: {
                    float dir = 0.0f;
                    if (Input::IsKeyPressed(b.positiveCode)) dir += 1.0f;
                    if (Input::IsKeyPressed(b.negativeCode)) dir -= 1.0f;
                    m_Value += dir;
                    break;
                }
                case Binding::Source::ControllerAxis: {
                    if (Input::IsControllerPresent(b.controllerId)) {
                        float raw = Input::GetControllerAxis(b.controllerId, b.code);
                        if (std::fabs(raw) > b.deadZone) {
                            m_Value += raw * b.scale;
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
            m_Value = std::clamp(m_Value, -1.0f, 1.0f);
            m_IsPressed = std::fabs(m_Value) > 0.1f;
        }
        else if (m_Type == Type::Axis2D) {
            for (auto& b : m_Bindings) {
                switch (b.source) {
                case Binding::Source::MousePosition:
                    m_Vec2Value = Input::GetMousePos();
                    break;
                default:
                    break;
                }
            }
        }
    }

    // --- InputActionManager ---

    InputActionManager& InputActionManager::Get() {
        static InputActionManager instance;
        return instance;
    }

    InputAction& InputActionManager::Create(const std::string& name, InputAction::Type type) {
        auto result = m_Actions.emplace(name, InputAction(name, type));
        return result.first->second;
    }

    InputAction* InputActionManager::Find(const std::string& name) {
        auto it = m_Actions.find(name);
        if (it != m_Actions.end())
            return &it->second;
        return nullptr;
    }

    InputAction& InputActionManager::Get(const std::string& name) {
        return m_Actions.at(name);
    }

    void InputActionManager::UpdateAll() {
        for (auto& pair : m_Actions) {
            pair.second.Update();
        }
    }

    bool InputActionManager::IsPressed(const std::string& name) {
        return Get().Get(name).IsPressed();
    }

    bool InputActionManager::IsDown(const std::string& name) {
        return Get().Get(name).IsDown();
    }

    bool InputActionManager::IsUp(const std::string& name) {
        return Get().Get(name).IsUp();
    }

    float InputActionManager::GetFloat(const std::string& name) {
        return Get().Get(name).GetFloat();
    }

    std::pair<float, float> InputActionManager::GetVec2(const std::string& name) {
        return Get().Get(name).GetVec2();
    }

} // namespace NFSEngine
