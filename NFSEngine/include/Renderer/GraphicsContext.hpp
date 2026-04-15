#pragma once

namespace NFSEngine {

    class GraphicContext {
    public:
        virtual ~GraphicContext() = default;
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
    };

} // namespace NFSEngine