#pragma once

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace NFSEngine {

    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual const std::string& GetName() const = 0;

        virtual void SetInt(const std::string& name, int value) = 0;
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
        virtual void SetVec3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetVec4(const std::string& name, const glm::vec4& value) = 0;

        static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    };
}


//namespace NFSEngine {
//
//    class Shader
//    {
//        public:
//        unsigned int ID;
//        
//        Shader(const char* vertexName, const char* fragmentName)
//        {
//            std::string vertexCode;
//            std::string fragmentCode;
//            std::ifstream vShaderFile;
//            std::ifstream fShaderFile;
//            
//            std::string vertexPath = std::string("assets/shaders/") + vertexName;
//            std::string fragmentPath = std::string("assets/shaders/") + fragmentName;
//            
//            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//            try
//            {
//                vShaderFile.open(vertexPath.c_str());
//                fShaderFile.open(fragmentPath.c_str());
//                
//                std::stringstream vShaderStream, fShaderStream;
//                
//                vShaderStream << vShaderFile.rdbuf();
//                fShaderStream << fShaderFile.rdbuf();
//                
//                vShaderFile.close();
//                fShaderFile.close();
//                
//                vertexCode = vShaderStream.str();
//                fragmentCode = fShaderStream.str();
//            }
//            catch (std::ifstream::failure& e)
//            {
//                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
//            }
//            
//            const char* vShaderCode = vertexCode.c_str();
//            const char* fShaderCode = fragmentCode.c_str();
//            
//            unsigned int vertex, fragment;
//            
//            vertex = glCreateShader(GL_VERTEX_SHADER);
//            glShaderSource(vertex, 1, &vShaderCode, NULL);
//            glCompileShader(vertex);
//            checkCompileErrors(vertex, "VERTEX");
//            
//            fragment = glCreateShader(GL_FRAGMENT_SHADER);
//            glShaderSource(fragment, 1, &fShaderCode, NULL);
//            glCompileShader(fragment);
//            checkCompileErrors(fragment, "FRAGMENT");
//            
//            ID = glCreateProgram();
//            glAttachShader(ID, vertex);
//            glAttachShader(ID, fragment);
//            glLinkProgram(ID);
//            checkCompileErrors(ID, "PROGRAM");
//            
//            glDeleteShader(vertex);
//            glDeleteShader(fragment);
//        }
//        
//        void use()
//        {
//            glUseProgram(ID);
//        }
//        
//        void setBool(const std::string& name, bool value) const
//        {
//            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
//        }
//        
//        void setInt(const std::string& name, int value) const
//        {
//            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
//        }
//        
//        void setFloat(const std::string& name, float value) const
//        {
//            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
//        }
//        
//        void setMat4(const std::string& name, const glm::mat4& mat) const
//        {
//            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//        }
//        
//        void setVec3(const std::string& name, const glm::vec3& value) const
//        {
//            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
//        }
//        
//        void setVec4(const std::string& name, const glm::vec4& value) const
//        {
//            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
//		}
//
//        private:
//        void checkCompileErrors(unsigned int shader, std::string type)
//        {
//            int success;
//            char infoLog[1024];
//            if (type != "PROGRAM")
//            {
//                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//                if (!success)
//                {
//                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
//                    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
//                }
//            }
//            else
//            {
//                glGetProgramiv(shader, GL_LINK_STATUS, &success);
//                if (!success)
//                {
//                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
//                    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
//                }
//            }
//        }
//    };
//
//}


