#include "WindowsInput.hpp"
#include "Engine.hpp"
#include <GLFW/glfw3.h>

Input* Input::instance = nullptr;

bool WindowsInput::IsKeyPressedImpl(int keycode) {
    auto window = glfwGetCurrentContext();
    if (!window) return false;

    int state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::IsMouseButtonPressedImpl(int button) {
    auto window = glfwGetCurrentContext();
    if (!window) return false;

    int state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::GetMousePosImpl() {
    auto window = glfwGetCurrentContext();
    if (!window) return {(float)0, (float)0};

    double xPos, yPos;

    glfwGetCursorPos(window, &xPos, &yPos);

    return {(float)xPos, (float)yPos};
}

float WindowsInput::GetMouseXImpl() {
    auto[x,y] = GetMousePosImpl();

    return (float)x;
}

float WindowsInput::GetMouseYImpl() {
    auto[x,y] = GetMousePosImpl();

    return (float)y;
}
