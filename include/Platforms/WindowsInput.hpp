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

        void UpdateStatesImpl() override;

    private:
        bool m_Keys[350] = { false };
        bool m_KeysLast[350] = { false };
        bool m_Mouse[8] = { false };
        bool m_MouseLast[8] = { false };
    };

}