# AGENTS.md — PBL_ZTGK (NFS Engine + Rhythm Game)

## Project structure

```
CMakeLists.txt          # Root CMake (C++17, min 3.20)
NFSEngine/              # Static library (custom game engine)
  CMakeLists.txt
  include/              # Public headers (mirrors src/ layout)
  src/                  # Engine implementation
    Core/               # Application, Layer, Scene, GameObject, Audio, Physics, etc.
    Renderer/           # Shader, Texture, Model, Frustum, etc.
    Platform/           # Window, Input
    UI/                 # Canvas, ButtonLogic, UIComponents, etc.
    ImGui/              # ImGuiLayer, SceneHierarchyPanel
    Components/         # Component, Transform, CubeMesh, AudioPatternComponent
  thirdparty/           # Vendored deps: imgui (docking), spdlog (v1.x), glad, stb, fmod, assimp, glm, nlohmann/json, tinygltf
Game/                   # Executable ("RythmGame")
  CMakeLists.txt
  src/                  # GameApp.cpp, LayerExample.cpp, UILayer.cpp
  include/              # GameStateView.hpp, LayerExample.hpp, UILayer.hpp
  assets/               # audio/, fonts/, models/, shaders/, textures/
```

## Build

```powershell
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Debug    # defines NFS_DEBUG
cmake --build build --config Release  # defines NFS_RELEASE
```

- Output binary: `build/Game/Debug/RythmGame.exe`
- FMOD & AssImp DLLs are copied automatically on Windows post-build
- Assets are copied from `Game/assets/` to the output dir via `CopyAssets` target

## Format

```powershell
cmake --build build --target format
# Runs clang-format -i on all .cpp/.h/.hpp in NFSEngine and Game
```

## Naming conventions (enforced by clang-tidy)

| Entity | Style | Example |
|---|---|---|
| Namespace | `CamelCase` | `NFSEngine` |
| Class/Struct/Enum | `CamelCase` | `Application`, `ApplicationConfig` |
| Function/Method | `CamelCase` | `CreateApplication`, `PushLayer` |
| Variable/Parameter | `camelBack` | `m_Window`, `config` |
| Private/Protected member | `m_PrefixedCamelCase` | `m_LayerStack`, `m_Config` |
| Global variable/pointer | `s_PrefixedCamelCase` | `s_Instance` |
| Global/static constant | `kPrefixedCamelCase` | |
| Enum constant | `CamelCase` | |

Namespace bodies are indented. Pointer/reference on left (`int* p`). Column limit 130. Spaces before parens only for control statements.

## Architecture notes

- **Entrypoint**: `Game/src/GameApp.cpp` defines `NFSEngine::CreateApplication()`. The `main()` lives in `NFSEngine/include/Core/EntryPoint.hpp` — it sets cwd to the exe dir, then calls `CreateApplication()`, runs profiling sessions around startup/runtime/shutdown.
- **Layer system**: `Application` owns a `LayerStack`. Game pushes layers and overlays (see `GameApp.cpp`). Layers are how the game extends the engine.
- **Defines**: `NFS_DEBUG` / `NFS_RELEASE` (set by build config). `NFS_PLATFORM_WINDOWS`, `NFS_PLATFORM_LINUX` auto-detected in `PlatformDetection.hpp`.

## Logging

```cpp
NFS_CORE_INFO("engine message");  // spdlog engine logger
NFS_INFO("game message");          // spdlog client logger
// TRACE, WARN, ERROR, CRITICAL variants exist for both
```

## Dependencies (Windows, prebuilt)

- FMOD: `thirdparty/fmod/lib/win/fmod_vc.lib` + `fmodstudio_vc.lib`; DLLs copied post-build
- AssImp: `thirdparty/assimp/lib/assimp-vc143-mt.lib` + `assimp-vc143-mt.dll`
- GLFW: fetched via FetchContent (3.3.8)
- ImGui: vendored, backend `imgui_impl_glfw` + `imgui_impl_opengl3`

## Commit style

`[PBL-XX] Short description` — conventional Jira-style prefix.

## Input action system

Game code never calls `Input::IsKeyPressed` etc. directly. Instead, **actions** are defined in `Game/src/GameApp.cpp` via `InputActionManager`:

```cpp
actions.Create("MoveX", InputAction::Type::Axis1D)
    .BindAxis1DKeys(Key::D, Key::A)
    .BindControllerAxis(ControllerAxes::LeftX, 0, 1.0f, 0.15f);
```

Consumers use `InputActionManager::IsPressed("Jump")`, `.IsDown("Dash")`, `.GetFloat("MoveX")`, etc.
- Button actions aggregate multiple bindings (any = pressed), track frame-state for Down/Up.
- Axis1D actions sum key-pair (±1) and controller axis contributions, clamped to [-1, 1].
- Actions are updated once per frame in `Application::Run()` right after `Input::UpdateStates()`.

## Profiling

The engine writes JSON profiling traces (`NFSEngine_Startup.json`, `NFSEngine_Runtime.json`, `NFSEngine_Shutdown.json`) next to the executable. See `Debug/Profiler.hpp`.
