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

        actions.Create("Jump", InputAction::Type::Button)
            .BindKey(Key::Space)
            .BindControllerButton(ControllerButtons::A);

        actions.Create("Dash", InputAction::Type::Button)
            .BindKey(Key::LeftShift)
            .BindControllerButton(ControllerButtons::RightBumper);

        actions.Create("ToggleCursor", InputAction::Type::Button)
            .BindKey(Key::Tab);

        actions.Create("LookX", InputAction::Type::Axis1D)
            .BindControllerAxis(ControllerAxes::RightX, 0, 1.0f, 0.15f);

        actions.Create("LookY", InputAction::Type::Axis1D)
            .BindControllerAxis(ControllerAxes::RightY, 0, 1.0f, 0.15f);
    }
}
