#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Renderer/Skybox.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/VertexArray.hpp"

namespace NFSEngine {
	class EnvironmentMap {
	public:
		EnvironmentMap();
		~EnvironmentMap() = default;

		bool LoadHDR(const std::string& filepath);

		void GenerateIrradiance(uint32_t cubeMapID);

		void GeneratePrefilterMap(uint32_t cubeMapID);
		void GenerateBRDFLUT();

		uint32_t GetIrradianceMapID() const { return m_IrradianceMap; }
		uint32_t GetEnvironmentMapID() const { return m_EnvironmentMap; }

		uint32_t GetPrefilterMapID() const { return m_PrefilterMap; }
		uint32_t GetBRDFLUTID() const { return m_BRDFLUT; }

		void BindEnvironmentMaps(uint32_t irradianceSlot = 5, uint32_t prefilterSlot = 6, uint32_t brdfSlot = 7) const;

	private:
		uint32_t m_IrradianceMap = 0;
		uint32_t m_EnvironmentMap = 0;
		uint32_t m_PrefilterMap = 0;
		uint32_t m_BRDFLUT = 0;

		uint32_t m_CaptureFBO = 0;
		uint32_t m_CaptureRBO = 0;

		std::shared_ptr<Shader> m_IrradianceShader;
		std::shared_ptr<Shader> m_EquirectangularShader;

		std::shared_ptr<Shader> m_PrefilterShader;
		std::shared_ptr<Shader> m_BRDFShader;

		std::shared_ptr<VertexArray> m_QuadVAO;
		void RenderQuad();
	};
}