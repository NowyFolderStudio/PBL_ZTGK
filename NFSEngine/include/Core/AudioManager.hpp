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
        static void ClearAllPatterns();

        static void SetActivePatternInTrack(const std::string& trackName, const std::string& patternName);
        static void SetTrackPitchOffset(const std::string& trackName, float pitchOffsetModifier);

        static void MuteTrack(const std::string& trackName, bool mute);
        static void SetTrackVolume(const std::string& trackName, float volume);

        static float GetDistanceToEventForTrack(const std::string& trackName);

    private:
        static std::unordered_map<std::string, std::vector<AudioPatternComponent*>> m_TrackGroups;
        static std::unordered_map<std::string, float> m_TrackDistances;

        static float CalculateTrackDistance(float songPosInBeats, const Pattern& activePattern);
    };

} // namespace NFSEngine