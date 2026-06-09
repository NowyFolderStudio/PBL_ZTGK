#include "Renderer/Shader.hpp"
#include "Platforms/OpenGL/OpenGLShader.hpp"

namespace NFSEngine {

    std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexPath,
                                           const std::string& fragmentPath, const std::string& geometryPath) {
        return std::make_shared<OpenGLShader>(name, vertexPath, fragmentPath, geometryPath);
    }

} // namespace NFSEngine