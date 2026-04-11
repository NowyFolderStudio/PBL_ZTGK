#include "Renderer/RendererAPI.hpp"
#include "Platforms/OpenGL/OpenGLRendererAPI.hpp"

namespace NFSEngine
{
std::unique_ptr<RendererAPI> RendererAPI::Create() { return std::make_unique<OpenGLRendererAPI>(); }
}