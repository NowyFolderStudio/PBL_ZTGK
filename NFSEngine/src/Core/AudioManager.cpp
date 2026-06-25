#include "Core/AudioManager.hpp"
#include <algorithm>
#include <cmath>

namespace NFSEngine {

    std::unordered_map<std::string, std::vector<AudioPatternComponent*>> AudioManager::m_TrackGroups;
    std::unordered_map<std::string, float> AudioManager::m_TrackDistances;

    void AudioManager::Init() { NFS_CORE_INFO("[AudioManager] Initialized tracks managing system."); }

    void AudioManager::Shutdown() {
        m_TrackGroups.clear();
        m_TrackDistances.clear();
        NFS_CORE_INFO("[AudioManager] Cleared assigned tracks and turned off system.");
    }

    void AudioManager::Update(DeltaTime deltaTime) {
        for (auto& pair : m_TrackGroups) {
            const std::string& trackName = pair.first;
            float closestDistance = 9999.0f;

            for (auto* comp : pair.second) {
                if (comp) {
                    comp->OnUpdate(deltaTime);

                    if (comp->IsActive) {
                        if (comp->GetSequencer()) {
                            float songPos = comp->GetSequencer()->GetContinuousBeatTime();
                            const Pattern& pattern = comp->GetPattern();
                            closestDistance = CalculateTrackDistance(songPos, pattern);
                        }
                    }
                }
            }

            m_TrackDistances[trackName] = closestDistance;
        }
    }

    void AudioManager::RegisterPattern(AudioPatternComponent* component) {
        if (component) {
            m_TrackGroups[component->TrackName].push_back(component);
            if (m_TrackDistances.find(component->TrackName) == m_TrackDistances.end()) {
                m_TrackDistances[component->TrackName] = 9999.0f;
            }
        }
    }

    void AudioManager::UnregisterPattern(AudioPatternComponent* component) {
        if (!component) return;
        auto& group = m_TrackGroups[component->TrackName];
        group.erase(std::remove(group.begin(), group.end(), component), group.end());
    }

    void AudioManager::ClearAllPatterns() {
        for (auto& pair : m_TrackGroups) {
            for (auto* comp : pair.second) {
                if (comp) {
                    comp->IsActive = false;
                    comp->SetMute(true);
                }
            }
        }

        m_TrackGroups.clear();
        m_TrackDistances.clear();
        NFS_CORE_INFO("[AudioManager] Cleared all registered track patterns.");
    }

    void AudioManager::SetActivePatternInTrack(const std::string& trackName, const std::string& patternName) {
        if (m_TrackGroups.find(trackName) == m_TrackGroups.end()) return;

        for (auto* comp : m_TrackGroups[trackName]) {
            if (comp->GetPatternName() == patternName) {
                comp->IsActive = true;
                NFS_CORE_INFO("[AudioManager] {} set to active on channel {}", patternName, trackName);
            } else {
                comp->IsActive = false;
            }
        }
    }

    void AudioManager::SetTrackPitchOffset(const std::string& trackName, float pitchOffsetModifier) {
        if (m_TrackGroups.find(trackName) == m_TrackGroups.end()) return;
        for (auto* comp : m_TrackGroups[trackName]) {
            comp->GlobalPitchModifier = pitchOffsetModifier;
        }
    }

    void AudioManager::MuteTrack(const std::string& trackName, bool mute) {
        if (m_TrackGroups.find(trackName) == m_TrackGroups.end()) return;
        for (auto* comp : m_TrackGroups[trackName]) {
            comp->SetMute(mute);
        }
    }

    void AudioManager::SetTrackVolume(const std::string& trackName, float volume) {
        if (m_TrackGroups.find(trackName) == m_TrackGroups.end()) return;
        for (auto* comp : m_TrackGroups[trackName]) {
            comp->SetVolume(volume);
        }
    }

    float AudioManager::GetDistanceToEventForTrack(const std::string& trackName) {
        auto it = m_TrackDistances.find(trackName);
        if (it != m_TrackDistances.end()) {
            return it->second;
        }
        return 9999.0f;
    }

    float AudioManager::CalculateTrackDistance(float songPosInBeats, const Pattern& activePattern) {
        const float beatsPerBar = 4.0f;
        float patternLengthBeats = activePattern.totalBars * beatsPerBar;

        if (patternLengthBeats <= 0.0f || activePattern.notes.empty()) return 9999.0f;

        float currentPatternPos = std::fmod(songPosInBeats, patternLengthBeats);
        if (currentPatternPos < 0.0f) currentPatternPos += patternLengthBeats;

        float minDistance = 9999.0f;

        for (const auto& note : activePattern.notes) {
            if (!note.isEvent) continue;

            float notePos = (note.bar - 1) * beatsPerBar + (note.beat - 1) + (note.sixteenth - 1) * 0.25f;

            float dist = std::abs(currentPatternPos - notePos);
            float wrapDistRight = std::abs(currentPatternPos - (notePos + patternLengthBeats));
            float wrapDistLeft = std::abs(currentPatternPos - (notePos - patternLengthBeats));

            float closest = std::min({ dist, wrapDistRight, wrapDistLeft });

            if (closest < minDistance) {
                minDistance = closest;
            }
        }

        return minDistance;
    }

} // namespace NFSEngine