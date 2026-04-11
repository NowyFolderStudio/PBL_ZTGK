#pragma once

#include <glad/glad.h>
#include "Renderer/Shader.hpp"

namespace NFSEngine {
	
	class OpenGLShader : public Shader {
	public:
        OpenGLShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
        virtual ~OpenGLShader();

        void Bind() const override;
        void Unbind() const override;

        const std::string& GetName() const override { return m_Name; }

        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        void SetInt(const std::string& name, int value) override;
        void SetFloat(const std::string& name, float value) override;
        void SetMat4(const std::string& name, const glm::mat4& value) override;
        void SetVec3(const std::string& name, const glm::vec3& value) override;
        void SetVec4(const std::string& name, const glm::vec4& value) override;

    private:
        std::string m_Name;
        uint32_t m_RendererID;

        void CheckCompileErrors(uint32_t shader, std::string type);
        std::string ReadFile(const std::string& filepath);
	};
}