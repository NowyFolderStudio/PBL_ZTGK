#include "Components/Component.hpp"
#include "Core/GameObject.hpp"
#include "Debug/Profiler.hpp"

namespace NFSEngine {

    void Component::Awake() {
        NFS_PROFILE_FUNCTION();
        if (m_Awakened) return;

        OnAwake();
        m_Awakened = true;
    }

    void Component::Start() {
        NFS_PROFILE_FUNCTION();
        if (m_Started || !m_Active) return;

        OnStart();
        m_Started = true;
    }

    void Component::FixedUpdate(DeltaTime fixedDeltaTime) {
        NFS_PROFILE_FUNCTION();
        if (!m_Active) return;

        if (!m_Started) {
            Start();
        }
        OnFixedUpdate(fixedDeltaTime);
    }

    void Component::Update(DeltaTime deltaTime) {
        NFS_PROFILE_FUNCTION();
        if (!m_Active) return;
        if (!m_Started) {
            Start();
        }
        OnUpdate(deltaTime);
    }

    void Component::Render() {
        NFS_PROFILE_FUNCTION();
        if (!m_Active) return;
        OnRender();
    }

    void Component::SetActive(bool isActive) {
        if (m_Active == isActive) return;

        m_Active = isActive;

        if (m_Active) {
            OnEnable();
        } else {
            OnDisable();
        }
    }

    std::string Component::GetOwnerName() const { return m_Owner ? m_Owner->name : "NONE"; }

    GameObject* Component::GetOwner() const { return m_Owner; }
}; // namespace NFSEngine
