#include "Renderer/Texture.hpp"
#include "Platforms/OpenGL/OpenGLTexture.hpp"

namespace NFSEngine
{

std::shared_ptr<Texture> Texture::Create(const std::string& path) { return std::make_shared<OpenGLTexture>(path); }

std::shared_ptr<Texture> Texture::Create(uint32_t width, uint32_t height)
{
    return std::make_shared<OpenGLTexture>(width, height);
}
}