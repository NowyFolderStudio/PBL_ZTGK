#pragma once

/*
GLFW keycodes if needed
Keyboard: https://www.glfw.org/docs/3.3/group__keys.html
Mouse: https://www.glfw.org/docs/latest/group__buttons.html
*/

namespace NFSEngine {

    using KeyCode = int;
    namespace Key {
        enum : KeyCode {
            Space         = 32,
            A             = 65,
            D             = 68,
            S             = 83,
            W             = 87,
            E             = 69,
            Q             = 81,
            Escape        = 256,
            LeftShift     = 340,
            LeftAlt       = 342,
            UP            = 265,
            DOWN          = 264,
            LEFT          = 263,
            RIGHT         = 262
        };
    }

    using MouseCode = int;
    namespace Mouse {
        enum : MouseCode {
            ButtonLeft   = 0,
            ButtonRight  = 1,
            ButtonMiddle = 2
        };
    }
}