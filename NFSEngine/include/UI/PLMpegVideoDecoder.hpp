#pragma once

#include "UI/VideoDecoder.hpp"
#include <pl_mpeg.h>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace NFSEngine {

    class PLMpegDecoder : public VideoDecoder {
    public:
        PLMpegDecoder() = default;

        ~PLMpegDecoder() override {
            StopThread();
            if (m_PLM) {
                plm_destroy(m_PLM);
            }
        }

        bool OpenFile(const std::string& path) override {
            StopThread();

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

            m_FrontBuffer.resize(m_Width * m_Height * 3);
            m_BackBuffer.resize(m_Width * m_Height * 3);
            m_FrameReady = false;

            StartThread(); // Odpalamy wątek w tle!

            return true;
        }

        bool ReadNextFrame() override {
            if (!m_FrameReady) return false;

            std::lock_guard<std::mutex> lock(m_Mutex);
            std::swap(m_FrontBuffer, m_BackBuffer);
            m_FrameReady = false;

            m_CV.notify_one();

            return true;
        }

        void* GetVideoData() override { return m_FrontBuffer.data(); }
        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        uint32_t GetDataSize() const override { return GetWidth() * GetHeight() * 3; }
        double GetFPS() const override { return m_FPS; }

        bool HasAudio() const override { return m_HasAudio; }
        void* GetAudioData() override { return nullptr; }
        size_t GetAudioSize() override { return 0; }

    private:
        void StartThread() {
            m_Running = true;
            m_DecodeThread = std::thread(&PLMpegDecoder::DecodeLoop, this);
        }

        void StopThread() {
            m_Running = false;
            m_CV.notify_one();
            if (m_DecodeThread.joinable()) {
                m_DecodeThread.join();
            }
        }

        void DecodeLoop() {
            int stride = m_Width * 3;

            while (m_Running) {
                std::unique_lock<std::mutex> lock(m_Mutex);

                m_CV.wait(lock, [this]() { return !m_FrameReady || !m_Running; });

                if (!m_Running) break;

                plm_frame_t* frame = plm_decode_video(m_PLM);

                if (frame) {
                    plm_frame_to_rgb(frame, m_BackBuffer.data(), stride);
                    m_FrameReady = true;
                }
            }
        }

        plm_t* m_PLM = nullptr;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        double m_FPS = 0.0;
        bool m_HasAudio = false;

        std::vector<uint8_t> m_FrontBuffer;
        std::vector<uint8_t> m_BackBuffer;

        std::thread m_DecodeThread;
        std::mutex m_Mutex;
        std::condition_variable m_CV;
        std::atomic<bool> m_Running { false };
        std::atomic<bool> m_FrameReady { false };
    };

} // namespace NFSEngine