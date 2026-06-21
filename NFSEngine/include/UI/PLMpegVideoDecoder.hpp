#pragma once

#include "UI/VideoDecoder.hpp"
#include <pl_mpeg.h>
#include <vector>
#include <string>

namespace NFSEngine {

    class PLMpegDecoder : public VideoDecoder {
    public:
        PLMpegDecoder() = default;

        ~PLMpegDecoder() override {
            if (m_PLM) {
                plm_destroy(m_PLM);
            }
        }

        bool OpenFile(const std::string& path) override {
            if (m_PLM) plm_destroy(m_PLM);

            m_PLM = plm_create_with_filename(path.c_str());
            if (!m_PLM) {
                NFS_CORE_ERROR("Can't load file: {}", path);
                return false;
            }
            plm_set_loop(m_PLM, 1);

            plm_set_audio_enabled(m_PLM, 0);

            m_Width = plm_get_width(m_PLM);
            m_Height = plm_get_height(m_PLM);
            m_FPS = plm_get_framerate(m_PLM);
            m_HasAudio = plm_get_num_audio_streams(m_PLM) > 0;

            m_RGBBuffer.resize(m_Width * m_Height * 3);

            return true;
        }

        bool ReadNextFrame() override {
            if (!m_PLM) return false;

            plm_frame_t* frame = plm_decode_video(m_PLM);

            if (!frame) return false;

            int stride = m_Width * 3;

            plm_frame_to_rgb(frame, m_RGBBuffer.data(), stride);

            return true;
        }

        void* GetVideoData() override { return m_RGBBuffer.data(); }
        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        double GetFPS() const override { return m_FPS; }

        bool HasAudio() const override { return m_HasAudio; }
        void* GetAudioData() override { return nullptr; }
        size_t GetAudioSize() override { return 0; }

    private:
        plm_t* m_PLM = nullptr;
        std::vector<uint8_t> m_RGBBuffer;

        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        double m_FPS = 0.0;
        bool m_HasAudio = false;
    };

} // namespace NFSEngine