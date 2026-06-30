#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "Core/DeltaTime.hpp"
#include "Core/Log.hpp"
#include "Renderer/Texture.hpp"
#include "UI/PLMpegVideoDecoder.hpp"
#include "UI/UIComponents.hpp"

namespace NFSEngine {

    class VideoComponent : public UIComponent {
    public:
        glm::vec4 Color = glm::vec4(1.0f); // Dodane
        std::shared_ptr<Texture> TexturePtr = nullptr; // Dodane i upublicznione

        float SpeedScale = 1.0f;
        bool IsPlaying = true;

        VideoComponent() { m_VideoDecoder = std::make_unique<PLMpegDecoder>(); }

        bool OpenFile(const std::string& path, bool loop = true) {
            if (m_VideoDecoder->OpenFile(path, loop)) {
                NFSEngine::TextureParameters texParams;
                texParams.Channels = 3;
                texParams.GenerateMipmaps = false;
                texParams.WrapS = NFSEngine::TextureWrap::Clamp;
                texParams.WrapT = NFSEngine::TextureWrap::Clamp;
                texParams.sRGB = false;

                TexturePtr = NFSEngine::Texture::Create(m_VideoDecoder->GetWidth(), m_VideoDecoder->GetHeight(), texParams);

                if (TexturePtr && m_VideoDecoder->ReadNextFrame()) {
                    uint32_t dataSize = m_VideoDecoder->GetDataSize();
                    TexturePtr->SetData(m_VideoDecoder->GetVideoData(), dataSize);
                }
                return true;
            }
            NFS_CORE_ERROR("[VideoComponent]: Can't load background video: {}", path);
            return false;
        }

        void Update(DeltaTime deltaTime) {
            if (!IsPlaying || !m_VideoDecoder || !TexturePtr) return;

            m_VideoAccumulator += deltaTime.GetSeconds() * SpeedScale;
            float frameTime = 1.0f / static_cast<float>(m_VideoDecoder->GetFPS());

            int framesDecodedThisTick = 0;

            while (m_VideoAccumulator >= frameTime && framesDecodedThisTick < 2) {
                if (m_VideoDecoder->ReadNextFrame()) {
                    uint32_t dataSize = m_VideoDecoder->GetDataSize();
                    TexturePtr->SetData(m_VideoDecoder->GetVideoData(), dataSize);

                    m_VideoAccumulator -= frameTime;
                    framesDecodedThisTick++;
                } else {
                    break;
                }
            }

            if (m_VideoAccumulator > frameTime * 2.0f) {
                m_VideoAccumulator = 0.0f;
            }
        }

    private:
        float m_VideoAccumulator = 0.0f;
        std::unique_ptr<PLMpegDecoder> m_VideoDecoder = nullptr;
    };

} // namespace NFSEngine