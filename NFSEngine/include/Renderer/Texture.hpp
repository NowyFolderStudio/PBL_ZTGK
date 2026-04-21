#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <cstdint>

namespace NFSEngine {

    class Texture {
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;

        virtual void SetData(void* data, uint32_t size) = 0;
        virtual void Bind(uint32_t slot = 0) const = 0;

        static std::shared_ptr<Texture> Create(const std::string& path);
        static std::shared_ptr<Texture> Create(uint32_t width, uint32_t height);
    };
} // namespace NFSEngine