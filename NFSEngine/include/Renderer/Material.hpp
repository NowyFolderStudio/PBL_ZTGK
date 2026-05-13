#pragma once

#include "Renderer/Texture.hpp"
#include "Renderer/Shader.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace NFSEngine {
	class Material {
	public:
		Material() = default;
		~Material() = default;

		std::shared_ptr<Texture> AlbedoMap = nullptr;
		std::shared_ptr<Texture> NormalMap = nullptr;
		std::shared_ptr<Texture> MetallicMap = nullptr;
		std::shared_ptr<Texture> RoughnessMap = nullptr;
		std::shared_ptr<Texture> AOMap = nullptr;

		glm::vec3 AlbedoColor = glm::vec3(1.0f);
		float Metallic = 0.0f;
		float Roughness = 1.0f;

		void Bind(const std::shared_ptr<Shader>& shader) const;
	};
}