#pragma once
#include <miniaudio/miniaudio.h>

namespace NFSEngine {
	class AudioEngine {
	public:
		static void Init();
		static void Shutdown();

		static ma_engine* GetEngine();

		static double GetGlobalTimeInSeconds();
		static ma_uint64 GetTimeInPCM();

	private:
		static ma_engine m_Engine;
	};
}