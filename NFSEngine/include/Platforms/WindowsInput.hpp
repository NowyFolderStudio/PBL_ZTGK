#pragma once

#include "Core/Input.hpp"

namespace NFSEngine {

    class WindowsInput : public Input {
    protected:
        bool IsKeyPressedImpl(int keycode) override;
        bool IsMouseButtonPressedImpl(int button) override;
        std::pair<float, float> GetMousePosImpl() override;
        float GetMouseXImpl() override;
        float GetMouseYImpl() override;

        bool IsKeyDownImpl(int keycode) override;
        bool IsKeyUpImpl(int keycode) override;
        bool IsMouseButtonDownImpl(int button) override;
        bool IsMouseButtonUpImpl(int button) override;

        bool IsControllerPresentImpl(int controllerId) override;
        bool IsControllerButtonPressedImpl(int controllerId, ControllerButton button) override;
        bool IsControllerButtonDownImpl(int controllerId, ControllerButton button) override;
        bool IsControllerButtonUpImpl(int controllerId, ControllerButton button) override;
        float GetControllerAxisImpl(int controllerId, ControllerAxis axis) override;

        void UpdateStatesImpl() override;

    private:
        bool m_Keys[512] = { false };
        bool m_KeysLast[512] = { false };
        bool m_Mouse[16] = { false };
        bool m_MouseLast[16] = { false };

        bool m_ControllerPresent[MAX_CONTROLLERS] = { false };
        bool m_ControllerButtons[MAX_CONTROLLERS][CONTROLLER_BUTTON_COUNT] = { false };
        bool m_ControllerButtonsLast[MAX_CONTROLLERS][CONTROLLER_BUTTON_COUNT] = { false };
        float m_ControllerAxes[MAX_CONTROLLERS][CONTROLLER_AXIS_COUNT] = { 0.0f };
    };

} // namespace NFSEngine