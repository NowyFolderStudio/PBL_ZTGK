#pragma once
#include <functional>
#include <unordered_map>

namespace NFSEngine {

    template <typename... Args> class Action {
    public:
        using Callback = std::function<void(Args...)>;

        size_t AddListener(Callback callback) {
            size_t id = ++m_NextId;
            m_Listeners[id] = std::move(callback);
            return id;
        }

        void RemoveListener(size_t id) { m_Listeners.erase(id); }

        void Invoke(Args... args) {
            auto listenersCopy = m_Listeners;

            for (const auto& [id, listener] : listenersCopy) {
                if (listener) {
                    listener(args...);
                }
            }
        }

        void Clear() { m_Listeners.clear(); }

    private:
        std::unordered_map<size_t, Callback> m_Listeners;
        size_t m_NextId = 0;
    };

} // namespace NFSEngine