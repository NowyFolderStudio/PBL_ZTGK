#include "Renderer/EnvironmentMap.hpp"
#include "Renderer/Renderer.hpp"
#include "Core/Application.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace NFSEngine {

    EnvironmentMap::EnvironmentMap() {
        m_IrradianceShader = Shader::Create("IrradianceShader", "assets/shaders/irradiance.vert", "assets/shaders/irradiance.frag");
    }

    void EnvironmentMap::GenerateIrradiance(const std::shared_ptr<Skybox>& skybox) {
        if (!skybox || !m_IrradianceShader) return;

        glGenFramebuffers(1, &m_CaptureFBO);
        glGenRenderbuffers(1, &m_CaptureRBO);

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

        skybox->Bind(0);

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

}