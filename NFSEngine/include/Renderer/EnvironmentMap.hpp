#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Renderer/Skybox.hpp"
#include "Renderer/Shader.hpp"

namespace NFSEngine {
	class EnvironmentMap {
	public:
		EnvironmentMap();
		~EnvironmentMap() = default;

		void GenerateIrradiance(const std::shared_ptr<Skybox>& skybox);

		uint32_t GetIrradianceMapID() const { return m_IrradianceMap; }

	private:
		uint32_t m_IrradianceMap = 0;
		uint32_t m_CaptureFBO = 0;
		uint32_t m_CaptureRBO = 0;

		std::shared_ptr<Shader> m_IrradianceShader;
	};
}