#include "Core/AudioManager.hpp"
#include <algorithm>
#include <iostream>

namespace NFSEngine {

	std::unordered_map<std::string, std::vector<AudioPatternComponent*>> AudioManager::m_TrackGroups;

	void AudioManager::Init() {
		std::cout << "[AudioManager] Zainicjalizowano system zarzadzania sciezkami." << std::endl;
	}

	void AudioManager::Shutdown() {
		m_TrackGroups.clear();
		std::cout << "[AudioManager] Wyczyszczono przypisane sciezki i wylaczono." << std::endl;
	}

	void AudioManager::Update(DeltaTime deltaTime) {
		for (auto& pair : m_TrackGroups) {
			for (auto* comp : pair.second) {
				if (comp) {
					comp->OnUpdate(deltaTime);
				}
			}
		}
	}

	void AudioManager::RegisterPattern(AudioPatternComponent* component) {
		if (component) {
			m_TrackGroups[component->TrackName].push_back(component);
		}
	}

	void AudioManager::UnregisterPattern(AudioPatternComponent* component) {
		if (!component) return;
		auto& group = m_TrackGroups[component->TrackName];
		group.erase(std::remove(group.begin(), group.end(), component), group.end());
	}

	void AudioManager::SetActivePatternInTrack(const std::string& trackName, const std::string& patternName) {
		if (m_TrackGroups.find(trackName) == m_TrackGroups.end()) return;

		for (auto* comp : m_TrackGroups[trackName]) {
			if (comp->GetPatternName() == patternName) {
				comp->IsActive = true;
				std::cout << "[AudioManager] Wlaczono: " << patternName << " na kanale " << trackName << std::endl;
			}
			else {
				comp->IsActive = false;
			}
		}
	}

	void AudioManager::MuteTrack(const std::string& trackName, bool mute) {
		if (m_TrackGroups.find(trackName) == m_TrackGroups.end()) return;
		for (auto* comp : m_TrackGroups[trackName]) {
			comp->IsActive = !mute;
		}
	}

	void AudioManager::SetTrackVolume(const std::string& trackName, float volume) {
		if (m_TrackGroups.find(trackName) == m_TrackGroups.end()) return;
		for (auto* comp : m_TrackGroups[trackName]) {
			comp->SetVolume(volume);
		}
	}
}