#include "Renderer/Framebuffer.hpp"
#include "Platforms/OpenGL/OpenGLFramebuffer.hpp"

namespace NFSEngine {

    std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) {
        // this should use some RendererApi to determine which Api to use
        return std::make_shared<OpenGLFramebuffer>(spec);
    }

} // namespace NFSEngine