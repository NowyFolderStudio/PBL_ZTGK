#include "Components/AudioPatternComponent.hpp"
#include "Events/NotePlayedEvent.hpp"
#include "Core/Application.hpp"
#include "Core/AudioManager.hpp"

#include <iostream>
#include <cmath>
#include <imgui.h>

namespace NFSEngine {

    AudioPatternComponent::~AudioPatternComponent() {
        for (auto& voice : m_Voices) {
            ma_sound_uninit(&voice);
        }
        NFSEngine::AudioManager::UnregisterPattern(this);
    }

    void AudioPatternComponent::LoadSound(const std::string& filepath) {
        NFS_CORE_INFO("Loading sounds for file: {}", filepath);

        int polyphonyCount = 4;
        m_Voices.resize(polyphonyCount);
        m_VoiceData.resize(polyphonyCount);

        bool anyLoaded = false;

        for (int i = 0; i < polyphonyCount; ++i) {
            ma_result result = ma_sound_init_from_file(AudioEngine::GetEngine(), filepath.c_str(), 0, NULL, NULL, &m_Voices[i]);

            if (result == MA_SUCCESS) {
                ma_sound_set_volume(&m_Voices[i], m_Volume);
                anyLoaded = true;
            } else {
                NFS_CORE_ERROR("Blad wczytywania glosu {}: {}", i, ma_result_description(result));
            }
        }

        m_IsLoaded = anyLoaded;
    }

    void AudioPatternComponent::LoadPattern(const std::string& patternFile, RhythmSequencer* sequencer) {
        m_Sequencer = sequencer;

        m_CurrentPattern = PatternParser::LoadFromFile(patternFile);

        if (!m_CurrentPattern.audioFile.empty()) {
            LoadSound(m_CurrentPattern.audioFile);
            NFS_CORE_INFO("Instrument loaded: {}", m_CurrentPattern.audioFile);
        }
    }

    void AudioPatternComponent::PlayNote(float pitchOffset, int lengthIn16ths) {
        if (!m_IsLoaded) return;

        if (m_IsMuted) return;

        float pitchMultiplier = std::pow(2.0f, pitchOffset / 12.0f);
        ma_sound* currentVoice = &m_Voices[m_CurrentVoiceIndex];

        ma_sound_stop(currentVoice);

        float safeVolume = m_Volume * 0.4f;
        ma_sound_set_fade_in_milliseconds(currentVoice, 0, safeVolume, 10);

        ma_sound_set_pitch(currentVoice, pitchMultiplier);
        ma_sound_seek_to_pcm_frame(currentVoice, 0);
        ma_sound_start(currentVoice);

        m_VoiceData[m_CurrentVoiceIndex].isPlaying = true;
        m_VoiceData[m_CurrentVoiceIndex].isFading = false;
        m_VoiceData[m_CurrentVoiceIndex].stop16thTotal = m_Sequencer->GetCurrent16thTotal() + lengthIn16ths;

        m_CurrentVoiceIndex = (m_CurrentVoiceIndex + 1) % m_Voices.size();
    }

    void AudioPatternComponent::OnUpdate(DeltaTime deltaTime) {
        if (!m_Sequencer || !m_IsLoaded || m_CurrentPattern.totalBars <= 0) return;

        int current16thTotal = m_Sequencer->GetCurrent16thTotal();

        for (size_t i = 0; i < m_Voices.size(); i++) {
            if (m_VoiceData[i].isPlaying) {

                if (!m_VoiceData[i].isFading && current16thTotal >= m_VoiceData[i].stop16thTotal) {
                    m_VoiceData[i].isFading = true;
                    m_VoiceData[i].fadeTimer = 0.1f;
                    float safeVolume = m_Volume * 0.4f;
                    ma_sound_set_fade_in_milliseconds(&m_Voices[i], safeVolume, 0.0f, 100);
                }

                if (m_VoiceData[i].isFading) {
                    m_VoiceData[i].fadeTimer -= deltaTime.GetSeconds();

                    if (m_VoiceData[i].fadeTimer <= 0.0f) {
                        ma_sound_stop(&m_Voices[i]);
                        m_VoiceData[i].isPlaying = false;
                        m_VoiceData[i].isFading = false;
                    }
                }
            }
        }

        if (current16thTotal > m_LastPlayed16thTotal) {

            if (IsActive) {
                int absoluteBar = m_Sequencer->GetCurrentBar();
                int currentBeat = m_Sequencer->GetBeatInBar();
                int current16th = m_Sequencer->Get16thInBeat();

                int localBar = ((absoluteBar - 1) % m_CurrentPattern.totalBars) + 1;

                for (const auto& note : m_CurrentPattern.notes) {
                    if (note.bar == localBar && note.beat == currentBeat && note.sixteenth == current16th) {

                        PlayNote(note.pitchOffset + GlobalPitchModifier, note.lengthIn16ths);

                        if (note.isEvent) {
                            NotePlayedEvent noteEvent(TrackName, m_CurrentPattern.name, note.noteName, absoluteBar, currentBeat,
                                                      current16th);
                            Application::Get().OnEvent(noteEvent);
                        }
                    }
                }
            }

            m_LastPlayed16thTotal = current16thTotal;
        }
    }

    void AudioPatternComponent::OnImGuiRender() {
        ImGui::Text("Zaladowany pattern: %s", m_CurrentPattern.name.c_str());

        if (ImGui::SliderFloat("Glosnosc", &m_Volume, 0.0f, 2.0f)) {

            if (m_IsLoaded) {
                for (auto& voice : m_Voices) {
                    ma_sound_set_volume(&voice, m_Volume);
                }
            }
        }
    }

    void AudioPatternComponent::SetVolume(float volume) {
        m_UnmutedVolume = volume;

        if (m_IsMuted) {
            m_Volume = 0.0f;
        } else {
            m_Volume = std::max(volume, 0.0f);
        }

        if (m_IsLoaded) {
            for (auto& voice : m_Voices) {
                ma_sound_set_volume(&voice, m_Volume);
            }
        }
    }

    void AudioPatternComponent::SetMute(bool mute) {
        m_IsMuted = mute;

        SetVolume(m_UnmutedVolume);
    }

    void AudioPatternComponent::SetActive(bool active) {
        if (IsActive == active) return;
        IsActive = active;

        if (IsActive && m_Sequencer) {
            m_LastPlayed16thTotal = m_Sequencer->GetCurrent16thTotal() - 1;
        }
    }
} // namespace NFSEngine