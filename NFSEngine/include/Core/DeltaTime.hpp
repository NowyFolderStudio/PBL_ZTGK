#pragma once

#include <math.h>

namespace NFSEngine {

    class DeltaTime {
        public:
            DeltaTime(float time = 0.0f) : m_Time(time) {}
        
            operator float() const { return m_Time; }

            float GetSeconds() const { return m_Time; }
            float GetMilliseconds() const {return m_Time * 1000.0f; }

            int GetFPS() const { 
                if (m_Time > 0.0f) {
                    return static_cast<int>(std::round(1.0f / m_Time)); 
                }
                return 0;
            }

        private:
            float m_Time;
    };

}