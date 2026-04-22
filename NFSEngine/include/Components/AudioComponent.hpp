#pragma once

#include "Components/Component.hpp"
#include "Core/AudioEngine.hpp"
#include <miniaudio/miniaudio.h>
#include <string>

namespace NFSEngine {
	class AudioComponent : public Component {
	public:
		AudioComponent(GameObject* owner) : Component(owner) {};
		~AudioComponent() override;

		void LoadSound(const std::string& filepath);
		void PlayScaleTest();

		std::string GetName() const override { return "AudioComponent"; }

	private:
		ma_sound m_Sound;
		bool m_IsLoaded = false;
	};
}