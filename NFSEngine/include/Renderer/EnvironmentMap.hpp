#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Renderer/Skybox.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"

namespace NFSEngine {
	class EnvironmentMap {
	public:
		EnvironmentMap();
		~EnvironmentMap() = default;

		bool LoadHDR(const std::string& filepath);

		void GenerateIrradiance(uint32_t cubeMapID);

		uint32_t GetIrradianceMapID() const { return m_IrradianceMap; }

		uint32_t GetEnvironmentMapID() const { return m_EnvironmentMap; }

	private:
		uint32_t m_IrradianceMap = 0;
		uint32_t m_EnvironmentMap = 0;

		uint32_t m_CaptureFBO = 0;
		uint32_t m_CaptureRBO = 0;

		std::shared_ptr<Shader> m_IrradianceShader;
		std::shared_ptr<Shader> m_EquirectangularShader;
	};
}