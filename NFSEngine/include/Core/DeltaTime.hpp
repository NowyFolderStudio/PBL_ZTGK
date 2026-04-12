#pragma once

namespace NFSEngine
{

class DeltaTime
{
public:
    DeltaTime(float time = 0.0f)
        : m_Time(time)
    {
    }

    operator float() const { return m_Time; }

    float GetSeconds() const { return m_Time; }
    float GetMilliseconds() const { return m_Time * 1000.0f; }

    float GetFPS() const
    {
        if (m_Time > 0.0f)
        {
            return 1.0f / m_Time;
        }
        return 0;
    }

private:
    float m_Time;
};

}