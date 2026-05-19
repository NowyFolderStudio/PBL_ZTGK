#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <variant>
#include <string>

#include "Renderer/Texture.hpp"
#include "Renderer/Shader.hpp"


namespace NFSEngine {

	using MaterialValue = std::variant<float, int, glm::vec3, glm::vec4>;

	class Material {
	public:
		Material() = default;
		~Material() = default;

		std::shared_ptr<Texture> AlbedoMap = nullptr;
		std::shared_ptr<Texture> NormalMap = nullptr;
		std::shared_ptr<Texture> MetallicMap = nullptr;
		std::shared_ptr<Texture> RoughnessMap = nullptr;
		std::shared_ptr<Texture> AOMap = nullptr;

		std::unordered_map<std::string, MaterialValue> Properties;

		glm::vec3 AlbedoColor = glm::vec3(1.0f);
		float Metallic = 0.0f;
		float Roughness = 1.0f;

		void Bind(const std::shared_ptr<Shader>& shader) const;

		void SetInt(const std::string& name, int value) { Properties[name] = value; }
		void SetFloat(const std::string& name, float value) { Properties[name] = value; }
		void SetVec4(const std::string& name, glm::vec4 value) { Properties[name] = value; }
		void SetVec3(const std::string& name, glm::vec3 value) { Properties[name] = value; }
	};
}