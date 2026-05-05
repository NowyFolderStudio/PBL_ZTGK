#include <iostream>

#define STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c" 

#define MINIAUDIO_IMPLEMENTATION
#include "Core/Audio/AudioEngine.hpp"

#undef STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"

namespace NFSEngine {
	ma_engine AudioEngine::m_Engine;

	void AudioEngine::Init() {
		ma_result result = ma_engine_init(NULL, &m_Engine);

		if (result != MA_SUCCESS) {
			std::cout << "Blad MiniAudio : " << result << std::endl;
		}
	}

	void AudioEngine::Shutdown() {
		ma_engine_uninit(&m_Engine);
		std::cout << "MiniAudio wylaczone " << std::endl;
	}

	ma_engine* AudioEngine::GetEngine() {
		return &m_Engine;
	}

	ma_uint64 AudioEngine::GetTimeInPCM() {
		return ma_engine_get_time_in_pcm_frames(&m_Engine);
	}

	double AudioEngine::GetGlobalTimeInSeconds() {
		ma_uint64 pcmFrames = GetTimeInPCM();
		ma_uint32 sampleRate = ma_engine_get_sample_rate(&m_Engine);

		return static_cast<double>(pcmFrames) / static_cast<double>(sampleRate);
	}
}