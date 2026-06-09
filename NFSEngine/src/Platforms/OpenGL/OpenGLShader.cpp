#include "Platforms/OpenGL/OpenGLShader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace NFSEngine {

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
        : m_Name(name) {

        std::string vertexCode = ReadFile(vertexPath);
        std::string fragmentCode = ReadFile(fragmentPath);

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");

        uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");

        uint32_t geometry = 0;
        bool hasGeometry = !geometryPath.empty();

        if (hasGeometry) {
            std::string geometryCode = ReadFile(geometryPath);
            const char* gShaderCode = geometryCode.c_str();

            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            CheckCompileErrors(geometry, "GEOMETRY");
        }

        m_RendererID = glCreateProgram();
        glAttachShader(m_RendererID, vertex);
        glAttachShader(m_RendererID, fragment);

        if (hasGeometry) {
            glAttachShader(m_RendererID, geometry);
        }

        glLinkProgram(m_RendererID);
        CheckCompileErrors(m_RendererID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (hasGeometry) {
            glDeleteShader(geometry);
        }
    }

    OpenGLShader::~OpenGLShader() { glDeleteProgram(m_RendererID); }

    std::string OpenGLShader::ReadFile(const std::string& filepath) {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in) {
            std::ostringstream contents;
            contents << in.rdbuf();
            result = contents.str();
        } else {
            NFS_CORE_ERROR("Could not open file: {}", filepath);
        }
        return result;
    }

    void OpenGLShader::Bind() const { glUseProgram(m_RendererID); }
    void OpenGLShader::Unbind() const { glUseProgram(0); }

    void OpenGLShader::SetInt(const std::string& name, int value) {
        glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
    }
    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) {
        glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }

    void OpenGLShader::SetFloat(const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
    }

    void OpenGLShader::SetVec2(const std::string& name, const glm::vec2& value) {
        glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
    }

    void OpenGLShader::SetVec3(const std::string& name, const glm::vec3& value) {
        glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
    }

    void OpenGLShader::SetVec4(const std::string& name, const glm::vec4& value) {
        glUniform4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
    }

    void OpenGLShader::SetBool(const std::string& name, const bool value) {
        glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), (int)value);
    }

    void OpenGLShader::SetMat4Array(const std::string& name, const std::vector<glm::mat4>& value) {
        glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), value.size(), GL_FALSE, glm::value_ptr(value[0]));
    }

    void OpenGLShader::CheckCompileErrors(uint32_t shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                NFS_CORE_ERROR("ERROR::SHADER::{}::COMPILATION ERROR: {}", m_Name, infoLog);
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                NFS_CORE_ERROR("ERROR::PROGRAM::{}::LINKING ERROR: {}", m_Name, infoLog);
            }
        }
    }
} // namespace NFSEngine