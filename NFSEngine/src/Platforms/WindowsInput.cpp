#include "Platforms/WindowsInput.hpp"
#include "Core/Application.hpp"
#include <GLFW/glfw3.h>
#include <cstring>

namespace NFSEngine {
    Input* Input::instance = nullptr;

    void WindowsInput::UpdateStatesImpl() {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        if (!window) return;

        std::memcpy(m_KeysLast, m_Keys, sizeof(m_Keys));
        std::memcpy(m_MouseLast, m_Mouse, sizeof(m_Mouse));

        for (int i = 32; i <= GLFW_KEY_LAST; i++) {
            m_Keys[i] = glfwGetKey(window, i) == GLFW_PRESS;
        }

        for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; i++) {
            m_Mouse[i] = glfwGetMouseButton(window, i) == GLFW_PRESS;
        }

        for (int c = 0; c < MAX_CONTROLLERS; c++) {
            std::memcpy(m_ControllerButtonsLast[c], m_ControllerButtons[c], sizeof(m_ControllerButtons[c]));

            int glfwId = GLFW_JOYSTICK_1 + c;
            m_ControllerPresent[c] = glfwJoystickIsGamepad(glfwId) == GLFW_TRUE;

            if (m_ControllerPresent[c]) {
                GLFWgamepadstate state;
                if (glfwGetGamepadState(glfwId, &state) == GLFW_TRUE) {
                    for (int b = 0; b < CONTROLLER_BUTTON_COUNT; b++) {
                        m_ControllerButtons[c][b] = state.buttons[b] == GLFW_PRESS;
                    }
                    for (int a = 0; a < CONTROLLER_AXIS_COUNT; a++) {
                        m_ControllerAxes[c][a] = state.axes[a];
                    }
                }
            }
        }
    }

    bool WindowsInput::IsKeyPressedImpl(KeyCode keycode) {
        if (keycode < 0 || keycode >= 512) return false;
        return m_Keys[keycode];
    }

    bool WindowsInput::IsMouseButtonPressedImpl(MouseCode button) {
        if (button < 0 || button >= 16) return false;
        return m_Mouse[button];
    }

    bool WindowsInput::IsKeyDownImpl(KeyCode keycode) {
        if (keycode < 0 || keycode >= 512) return false;
        return m_Keys[keycode] && !m_KeysLast[keycode];
    }

    bool WindowsInput::IsKeyUpImpl(KeyCode keycode) {
        if (keycode < 0 || keycode >= 512) return false;
        return !m_Keys[keycode] && m_KeysLast[keycode];
    }

    bool WindowsInput::IsMouseButtonDownImpl(MouseCode button) {
        if (button < 0 || button >= 16) return false;
        return m_Mouse[button] && !m_MouseLast[button];
    }

    bool WindowsInput::IsMouseButtonUpImpl(MouseCode button) {
        if (button < 0 || button >= 16) return false;
        return !m_Mouse[button] && m_MouseLast[button];
    }

    std::pair<float, float> WindowsInput::GetMousePosImpl() {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        if (!window) return { 0.0f, 0.0f };

        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        return { static_cast<float>(xPos), static_cast<float>(yPos) };
    }

    float WindowsInput::GetMouseXImpl() { return GetMousePosImpl().first; }

    float WindowsInput::GetMouseYImpl() { return GetMousePosImpl().second; }

    bool WindowsInput::IsControllerPresentImpl(int controllerId) {
        if (controllerId < 0 || controllerId >= MAX_CONTROLLERS) return false;
        return m_ControllerPresent[controllerId];
    }

    bool WindowsInput::IsControllerButtonPressedImpl(int controllerId, ControllerButton button) {
        if (controllerId < 0 || controllerId >= MAX_CONTROLLERS) return false;
        if (button < 0 || button >= CONTROLLER_BUTTON_COUNT) return false;
        return m_ControllerButtons[controllerId][button];
    }

    bool WindowsInput::IsControllerButtonDownImpl(int controllerId, ControllerButton button) {
        if (controllerId < 0 || controllerId >= MAX_CONTROLLERS) return false;
        if (button < 0 || button >= CONTROLLER_BUTTON_COUNT) return false;
        return m_ControllerButtons[controllerId][button] && !m_ControllerButtonsLast[controllerId][button];
    }

    bool WindowsInput::IsControllerButtonUpImpl(int controllerId, ControllerButton button) {
        if (controllerId < 0 || controllerId >= MAX_CONTROLLERS) return false;
        if (button < 0 || button >= CONTROLLER_BUTTON_COUNT) return false;
        return !m_ControllerButtons[controllerId][button] && m_ControllerButtonsLast[controllerId][button];
    }

    float WindowsInput::GetControllerAxisImpl(int controllerId, ControllerAxis axis) {
        if (controllerId < 0 || controllerId >= MAX_CONTROLLERS) return 0.0f;
        if (axis < 0 || axis >= CONTROLLER_AXIS_COUNT) return 0.0f;
        return m_ControllerAxes[controllerId][axis];
    }

} // namespace NFSEngine