#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <string>

namespace NFSEngine {
    class BenchmarkTool {
    public:
        void Start(const std::string& testName) {
            m_TestName = testName;
            m_IsRunning = true;
            m_FrameTimes.clear();
            m_FrameTimes.reserve(10000);
            m_StartTime = std::chrono::steady_clock::now();
        }

        void RecordFrame(double deltaTimeSeconds) {
            if (!m_IsRunning) return;
            m_FrameTimes.push_back(deltaTimeSeconds);
        }

        void StopAndSave() {
            if (!m_IsRunning || m_FrameTimes.empty()) return;
            m_IsRunning = false;

            auto endTime = std::chrono::steady_clock::now();
            std::chrono::duration<double> totalTime = endTime - m_StartTime;

            double avgFPS = m_FrameTimes.size() / totalTime.count();

            std::sort(m_FrameTimes.begin(), m_FrameTimes.end());

            size_t onePercentCount = std::max<size_t>(1, m_FrameTimes.size() / 100);
            double sumWorst1Percent = 0.0;

            for (size_t i = m_FrameTimes.size() - onePercentCount; i < m_FrameTimes.size(); ++i) {
                sumWorst1Percent += m_FrameTimes[i];
            }

            double avgFrameTimeWorst1Percent = sumWorst1Percent / onePercentCount;
            double onePercentLowFPS = 1.0 / avgFrameTimeWorst1Percent;

            std::ofstream file(m_TestName + "_results.csv");
            if (file.is_open()) {
                file << "TestName,TotalFrames,TotalTimeSec,AvgFPS,1%LowFPS\n";
                file << m_TestName << "," << m_FrameTimes.size() << "," << totalTime.count() << "," << avgFPS << ","
                     << onePercentLowFPS << "\n";
                file.close();
            }
        }

    private:
        bool m_IsRunning = false;
        std::string m_TestName;
        std::vector<double> m_FrameTimes;
        std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
    };
} // namespace NFSEngine
