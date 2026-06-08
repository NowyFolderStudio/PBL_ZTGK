#pragma once

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace NFSEngine {

    class Shader {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual const std::string& GetName() const = 0;

        virtual uint32_t GetRendererID() const = 0;

        virtual void SetInt(const std::string& name, int value) = 0;
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
        virtual void SetVec2(const std::string& name, const glm::vec2& value) = 0;
        virtual void SetVec3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetVec4(const std::string& name, const glm::vec4& value) = 0;
        virtual void SetBool(const std::string& name, bool value) = 0;
        virtual void SetMat4Array(const std::string& name, const std::vector<glm::mat4>& value) = 0;

        static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexPath,
                                              const std::string& fragmentPath);
    };
} // namespace NFSEngine
