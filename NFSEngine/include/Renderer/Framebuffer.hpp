#pragma once
#include <cstdint>
#include <vector>
#include <memory>

namespace NFSEngine {

    enum class FramebufferTextureFormat {
        None = 0,
        RGBA8,
        RED_INTEGER,
        DEPTH24STENCIL8,
        Depth = DEPTH24STENCIL8
    };

    struct FramebufferTextureSpecification {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(FramebufferTextureFormat format)
            : textureFormat(format) { }

        FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
    };

    struct FramebufferAttachmentSpecification {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
            : attachments(attachments) { }

        std::vector<FramebufferTextureSpecification> attachments;
    };

    struct FramebufferSpecification {
        uint32_t width = 0;
        uint32_t height = 0;
        FramebufferAttachmentSpecification attachments;
        uint32_t samples = 1;

        bool swapChainTarget = false;
    };

    class Framebuffer {
    public:
        virtual ~Framebuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;
        virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
        virtual const FramebufferSpecification& GetSpecification() const = 0;

        static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& spec);
    };

} // namespace NFSEngine