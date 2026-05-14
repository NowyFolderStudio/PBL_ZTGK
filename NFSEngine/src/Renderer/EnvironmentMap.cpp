#include "Renderer/EnvironmentMap.hpp"
#include "Renderer/Renderer.hpp"
#include "Core/Application.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace NFSEngine {

    EnvironmentMap::EnvironmentMap() {
        m_IrradianceShader = Shader::Create("IrradianceShader", "assets/shaders/irradiance.vert", "assets/shaders/irradiance.frag");
        m_EquirectangularShader = Shader::Create("EquiShader", "assets/shaders/irradiance.vert", "assets/shaders/equirectangular.frag");
        m_PrefilterShader = Shader::Create("PrefilterShader", "assets/shaders/irradiance.vert", "assets/shaders/prefilter.frag");
        m_BRDFShader = Shader::Create("BRDFShader", "assets/shaders/brdf.vert", "assets/shaders/brdf.frag");

        glGenFramebuffers(1, &m_CaptureFBO);
        glGenRenderbuffers(1, &m_CaptureRBO);
    }

    void EnvironmentMap::RenderQuad() {
        if (!m_QuadVAO) {
            float quadVertices[] = {
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            m_QuadVAO = std::shared_ptr<VertexArray>(VertexArray::Create());
            auto vbo = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(quadVertices, sizeof(quadVertices)));
            vbo->SetLayout({
                {ShaderDataType::Float3, "aPos"},
                {ShaderDataType::Float2, "aTexCoords" }
                });
            m_QuadVAO->AddVertexBuffer(vbo);
        }
        m_QuadVAO->Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    bool EnvironmentMap::LoadHDR(const std::string& filepath) {
        auto hdrTexture = Texture::Create(filepath);
        if (hdrTexture->GetRendererID() == 0) return false;

        glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_CaptureRBO);

        glGenTextures(1, &m_EnvironmentMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvironmentMap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] = {
            glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        m_EquirectangularShader->Bind();
        m_EquirectangularShader->SetInt("equirectangularMap", 0);
        m_EquirectangularShader->SetMat4("projection", captureProjection);

        hdrTexture->Bind(0);

        glViewport(0, 0, 512, 512);
        glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);

        for (unsigned int i = 0; i < 6; ++i) {
            m_EquirectangularShader->SetMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_EnvironmentMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Renderer::GetSkyboxVAO()->Bind();
            Renderer::GetAPI().DrawArrays(Renderer::GetSkyboxVAO(), 36);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Application::Get().GetConfig().WindowWidth, Application::Get().GetConfig().WindowHeight);

        return true;
    }

    void EnvironmentMap::GenerateIrradiance(uint32_t sourceCubeMapID) {
        if (sourceCubeMapID == 0 || !m_IrradianceShader) return;

        glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_CaptureRBO);

        glGenTextures(1, &m_IrradianceMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceMap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] = {
            glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        m_IrradianceShader->Bind();
        m_IrradianceShader->SetInt("environmentMap", 0);
        m_IrradianceShader->SetMat4("projection", captureProjection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sourceCubeMapID);

        glViewport(0, 0, 32, 32);
        glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);

        for (unsigned int i = 0; i < 6; ++i) {
            m_IrradianceShader->SetMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_IrradianceMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Renderer::GetSkyboxVAO()->Bind();
            Renderer::GetAPI().DrawArrays(Renderer::GetSkyboxVAO(), 36);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Application::Get().GetConfig().WindowWidth, Application::Get().GetConfig().WindowHeight);
    }

    void EnvironmentMap::GeneratePrefilterMap(uint32_t sourceCubeMapID) {
        if (sourceCubeMapID == 0 || !m_PrefilterShader) return;

        glGenTextures(1, &m_PrefilterMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_PrefilterMap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] = {
            glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        m_PrefilterShader->Bind();
        m_PrefilterShader->SetInt("environmentMap", 0);
        m_PrefilterShader->SetMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sourceCubeMapID);

        glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);

        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            unsigned int mipWidth = 128 * std::pow(0.5, mip);
            unsigned int mipHeight = 128 * std::pow(0.5, mip);
            glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            m_PrefilterShader->SetFloat("roughness", roughness);

            for (unsigned int i = 0; i < 6; ++i) {
                m_PrefilterShader->SetMat4("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_PrefilterMap, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                Renderer::GetSkyboxVAO()->Bind();
                Renderer::GetAPI().DrawArrays(Renderer::GetSkyboxVAO(), 36);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Application::Get().GetConfig().WindowWidth, Application::Get().GetConfig().WindowHeight);
    }

    void EnvironmentMap::GenerateBRDFLUT() {
        if (!m_BRDFShader) return;

        glGenTextures(1, &m_BRDFLUT);
        glBindTexture(GL_TEXTURE_2D, m_BRDFLUT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BRDFLUT, 0);

        glViewport(0, 0, 512, 512);
        m_BRDFShader->Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Application::Get().GetConfig().WindowWidth, Application::Get().GetConfig().WindowHeight);
    }

    void EnvironmentMap::BindEnvironmentMaps(uint32_t irradianceSlot, uint32_t prefilterSlot, uint32_t brdfSlot) const {
        if (m_IrradianceMap > 0) {
            glActiveTexture(GL_TEXTURE0 + irradianceSlot);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceMap);
        }

        if (m_PrefilterMap > 0) {
            glActiveTexture(GL_TEXTURE0 + prefilterSlot);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_PrefilterMap);
        }

        if (m_BRDFLUT > 0) {
            glActiveTexture(GL_TEXTURE0 + brdfSlot);
            glBindTexture(GL_TEXTURE_2D, m_BRDFLUT);
        }
    }
}