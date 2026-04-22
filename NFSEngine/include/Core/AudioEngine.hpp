#pragma once
#include <miniaudio/miniaudio.h>

namespace NFSEngine {
	class AudioEngine {
	public:
		static void Init();
		static void Shutdown();

		static ma_engine* GetEngine();

	private:
		static ma_engine m_Engine;
	};
}