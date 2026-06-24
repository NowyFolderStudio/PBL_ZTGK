#include "nfspch.h"

namespace NFSEngine {
    class VideoDecoder {
    public:
        virtual ~VideoDecoder() = default;

        virtual bool OpenFile(const std::string& path, bool loop = true) = 0;
        virtual bool ReadNextFrame() = 0;

        virtual bool IsFinished() const = 0;

        virtual void* GetVideoData() = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetDataSize() const = 0;
        virtual double GetFPS() const = 0;

        virtual bool HasAudio() const { return false; }
        virtual void* GetAudioData() { return nullptr; }
        virtual size_t GetAudioSize() { return 0; }
    };
} // namespace NFSEngine