#pragma once
#include "Renderer/Skybox.hpp"
#include <glad/glad.h>

namespace NFSEngine {
    class OpenGLSkybox : public Skybox {
    public:
        OpenGLSkybox(const std::vector<std::string>& facePaths);
        virtual ~OpenGLSkybox();

        void Bind(uint32_t slot = 0) const override;
        uint32_t GetRendererID() const override { return m_RendererID; }

    private:
        uint32_t m_RendererID;
    };
}