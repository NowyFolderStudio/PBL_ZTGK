#include <iostream>

#define STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c" 

#define MINIAUDIO_IMPLEMENTATION
#include "Core/AudioEngine.hpp"

#undef STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"

namespace NFSEngine {
	ma_engine AudioEngine::m_Engine;

	void AudioEngine::Init() {
		ma_result result = ma_engine_init(NULL, &m_Engine);

		if (result != MA_SUCCESS) {
			std::cout << "Blad MiniAudio : " << result << std::endl;
		}
		else {
			std::cout << "MiniAudio dziala poprawnie" << std::endl;
		}
	}

	void AudioEngine::Shutdown() {
		ma_engine_uninit(&m_Engine);
		std::cout << "MiniAudio wylaczone " << std::endl;
	}

	ma_engine* AudioEngine::GetEngine() {
		return &m_Engine;
	}
}