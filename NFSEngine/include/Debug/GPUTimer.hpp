#pragma once

#include <glad/glad.h>

namespace NFSEngine {

    class GPUTimer {
    public:
        GPUTimer() { glGenQueries(1, &m_QueryId); }
        ~GPUTimer() { glDeleteQueries(1, &m_QueryId); }

        void Begin() const { glBeginQuery(GL_TIME_ELAPSED, m_QueryId); }
        void End() { glEndQuery(GL_TIME_ELAPSED); }

        float GetTimeMS() const {
            GLuint64 timeNanoseconds = 0;
            glGetQueryObjectui64v(m_QueryId, GL_QUERY_RESULT, &timeNanoseconds);
            return static_cast<float>(timeNanoseconds) / 1000000.0f;
        }

    private:
        uint32_t m_QueryId;
    };

} // namespace NFSEngine