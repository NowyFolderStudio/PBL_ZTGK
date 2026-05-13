#include "Renderer/Material.hpp"

namespace NFSEngine {

	void Material::Bind(const std::shared_ptr<Shader>& shader) const {
		if (!shader) return;

        if (AlbedoMap) {
            AlbedoMap->Bind(0);
            shader->SetInt("u_AlbedoMap", 0);
            shader->SetBool("u_HasAlbedoMap", true);
        }
        else {
            shader->SetBool("u_HasAlbedoMap", false);
            shader->SetVec3("u_AlbedoColor", AlbedoColor);
        }

        if (NormalMap) {
            NormalMap->Bind(1);
            shader->SetInt("u_NormalMap", 1);
            shader->SetBool("u_HasNormalMap", true);
        }
        else {
            shader->SetBool("u_HasNormalMap", false);
        }

        if (MetallicMap) {
            MetallicMap->Bind(2);
            shader->SetInt("u_MetallicMap", 2);
            shader->SetBool("u_HasMetallicMap", true);
        }
        else {
            shader->SetBool("u_HasMetallicMap", false);
            shader->SetFloat("u_Metallic", Metallic);
        }

        if (RoughnessMap) {
            RoughnessMap->Bind(3);
            shader->SetInt("u_RoughnessMap", 3);
            shader->SetBool("u_HasRoughnessMap", true);
        }
        else {
            shader->SetBool("u_HasRoughnessMap", false);
            shader->SetFloat("u_Roughness", Roughness);
        }

        if (AOMap) {
            AOMap->Bind(4);
            shader->SetInt("u_AOMap", 4);
            shader->SetBool("u_HasAOMap", true);
        }
        else {
            shader->SetBool("u_HasAOMap", false);
        }
	}
}