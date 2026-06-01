#pragma once
#include "Components/AudioPatternComponent.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace NFSEngine {

	class AudioManager {
	public:
		static void Init();
		static void Shutdown();

		static void Update(DeltaTime deltaTime);

		static void RegisterPattern(AudioPatternComponent* component);
		static void UnregisterPattern(AudioPatternComponent* component);

		static void SetActivePatternInTrack(const std::string& trackName, const std::string& patternName);

		static void MuteTrack(const std::string& trackName, bool mute);
		static void SetTrackVolume(const std::string& trackName, float volume);

	private:
		static std::unordered_map<std::string, std::vector<AudioPatternComponent*>> m_TrackGroups;
	};

}