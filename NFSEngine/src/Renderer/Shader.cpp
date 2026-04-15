#include "Renderer/Shader.hpp"
#include "Platforms/OpenGL/OpenGLShader.hpp"

namespace NFSEngine {

    std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexPath,
                                           const std::string& fragmentPath) {
        return std::make_shared<OpenGLShader>(name, vertexPath, fragmentPath);
    }

} // namespace NFSEngine