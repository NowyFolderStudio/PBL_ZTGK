#pragma once
#include <string>
#include <utility>
#include <miniaudio/miniaudio.h>

namespace NFSEngine {
    class AudioClip {
    public:
        // Use stream = true for longer audio clips to save up RAM
        // Use stream = false for short clips
        AudioClip(std::string filepath, bool stream = false)
            : m_Filepath(std::move(filepath))
            , m_Stream(stream) { }

        const std::string& GetFilepath() const { return m_Filepath; }
        bool IsStream() const { return m_Stream; }

    private:
        std::string m_Filepath;
        bool m_Stream;
    };
} // namespace NFSEngine