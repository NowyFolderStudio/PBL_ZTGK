#include "InputActions.hpp"
#include <NFSEngine.h>

namespace NFSEngine {
    void RegisterInputActions() {
        auto& actions = InputActionManager::Get();

        actions.Create("MoveX", InputAction::Type::Axis1D)
            .BindAxis1DKeys(Key::D, Key::A)
            .BindControllerAxis(ControllerAxes::LeftX, 0, 1.0f, 0.15f);

        actions.Create("MoveZ", InputAction::Type::Axis1D)
            .BindAxis1DKeys(Key::W, Key::S)
            .BindControllerAxis(ControllerAxes::LeftY, 0, -1.0f, 0.15f);

        actions.Create("Jump", InputAction::Type::Button).BindKey(Key::Space).BindControllerButton(ControllerButtons::A);

        actions.Create("Dash", InputAction::Type::Button).BindKey(Key::LeftShift).BindControllerButton(ControllerButtons::X);

        actions.Create("ToggleCursor", InputAction::Type::Button).BindKey(Key::Tab);

        actions.Create("LookX", InputAction::Type::Axis1D).BindControllerAxis(ControllerAxes::RightX, 0, 1.0f, 0.15f);

        actions.Create("LookY", InputAction::Type::Axis1D).BindControllerAxis(ControllerAxes::RightY, 0, 1.0f, 0.15f);

        actions.Create("Attack", InputAction::Type::Button).BindKey(Key::F);

        actions.Create("SelectFirstAura", InputAction::Type::Button)
            .BindKey(Key::Q)
            .BindControllerButton(ControllerButtons::LeftBumper);

        actions.Create("SelectSecondAura", InputAction::Type::Button)
            .BindKey(Key::E)
            .BindControllerButton(ControllerButtons::RightBumper);

        actions.Create("UINavUp", InputAction::Type::Button)
            .BindKey(Key::Up)
            .BindKey(Key::W)
            .BindControllerButton(ControllerButtons::DPadUp);

        actions.Create("UINavDown", InputAction::Type::Button)
            .BindKey(Key::Down)
            .BindKey(Key::S)
            .BindControllerButton(ControllerButtons::DPadDown);

        actions.Create("UINavLeft", InputAction::Type::Button)
            .BindKey(Key::Left)
            .BindKey(Key::A)
            .BindControllerButton(ControllerButtons::DPadLeft);

        actions.Create("UINavRight", InputAction::Type::Button)
            .BindKey(Key::Right)
            .BindKey(Key::D)
            .BindControllerButton(ControllerButtons::DPadRight);

        actions.Create("UIConfirm", InputAction::Type::Button).BindKey(Key::Enter).BindControllerButton(ControllerButtons::A);
    }
} // namespace NFSEngine