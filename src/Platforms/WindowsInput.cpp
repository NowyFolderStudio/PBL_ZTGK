#include "Platforms/WindowsInput.hpp"
#include <GLFW/glfw3.h>

namespace NFSEngine {

    Input* Input::instance = nullptr;

    void WindowsInput::UpdateStatesImpl() {
        auto window = glfwGetCurrentContext();
        if (!window) return;

        // 1. Przesuwamy obecny stan do "poprzedniego"
        for (int i = 0; i < 350; i++) m_KeysLast[i] = m_Keys[i];
        for (int i = 0; i < 8; i++) m_MouseLast[i] = m_Mouse[i];

        // 2. Pobieramy aktualny stan z GLFW
        // Zakres klawiszy 32-348 pokrywa większość standardowych znaków i funkcyjnych
        for (int i = 32; i < 349; i++) {
            m_Keys[i] = glfwGetKey(window, i) == GLFW_PRESS;
        }

        for (int i = 0; i < 8; i++) {
            m_Mouse[i] = glfwGetMouseButton(window, i) == GLFW_PRESS;
        }
    }

    // --- Metody ciągłego sprawdzania (zachowanie wsteczne) ---
    bool WindowsInput::IsKeyPressedImpl(int keycode) {
        return m_Keys[keycode];
    }

    bool WindowsInput::IsMouseButtonPressedImpl(int button) {
        return m_Mouse[button];
    }

    // --- Nowe metody (detekcja zbocza sygnału) ---
    bool WindowsInput::IsKeyDownImpl(int keycode) {
        return m_Keys[keycode] && !m_KeysLast[keycode]; // Teraz jest, wczoraj nie było
    }

    bool WindowsInput::IsKeyUpImpl(int keycode) {
        return !m_Keys[keycode] && m_KeysLast[keycode]; // Teraz nie ma, wczoraj było
    }

    bool WindowsInput::IsMouseButtonDownImpl(int button) {
        return m_Mouse[button] && !m_MouseLast[button];
    }

    bool WindowsInput::IsMouseButtonUpImpl(int button) {
        return !m_Mouse[button] && m_MouseLast[button];
    }

    std::pair<float, float> WindowsInput::GetMousePosImpl() {
        auto window = glfwGetCurrentContext();
        if (!window) return { 0.0f, 0.0f };
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        return { (float)xPos, (float)yPos };
    }

    float WindowsInput::GetMouseXImpl() { return GetMousePosImpl().first; }
    float WindowsInput::GetMouseYImpl() { return GetMousePosImpl().second; }

}