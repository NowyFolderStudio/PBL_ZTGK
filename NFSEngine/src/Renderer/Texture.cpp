#include "Renderer/Texture.hpp"
#include "Platforms/OpenGL/OpenGLTexture.hpp"

namespace NFSEngine {

    std::shared_ptr<Texture> Texture::Create(const std::string& path, const TextureParameters& params) {
        return std::make_shared<OpenGLTexture>(path, params);
    }

    std::shared_ptr<Texture> Texture::Create(uint32_t width, uint32_t height, const TextureParameters& params) {
        return std::make_shared<OpenGLTexture>(width, height, params);
    }
} // namespace NFSEngine