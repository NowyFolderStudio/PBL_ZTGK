#pragma once
#include <string>
#include <vector>
#include <memory>

namespace NFSEngine {
    class Skybox {
    public:
        virtual ~Skybox() = default;
        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual uint32_t GetRendererID() const = 0;

        // Wants 6-path Vector: right, left, up, down, front, back
        static std::shared_ptr<Skybox> Create(const std::vector<std::string>& facePaths);
    };
}