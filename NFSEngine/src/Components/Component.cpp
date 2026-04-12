#include "Components/Component.hpp"
#include "Core/GameObject.hpp"

namespace NFSEngine
{

void Component::Awake()
{
    if (m_Awakened) return;

    OnAwake();
    m_Awakened = true;
}

void Component::Start()
{
    if (m_Started || !m_Active) return;

    OnStart();
    m_Started = true;
}

void Component::FixedUpdate(DeltaTime fixedDeltaTime)
{
    if (!m_Active) return;

    if (!m_Started)
    {
        Start();
    }
    OnFixedUpdate(fixedDeltaTime);
}

void Component::Update(DeltaTime deltaTime)
{
    if (!m_Active) return;
    if (!m_Started)
    {
        Start();
    }
    OnUpdate(deltaTime);
}

void Component::Render()
{
    if (!m_Active) return;
    OnRender();
}

void Component::SetActive(bool isActive)
{
    if (m_Active == isActive) return;

    m_Active = isActive;

    if (m_Active)
    {
        OnEnable();
    }
    else
    {
        OnDisable();
    }
}

std::string Component::GetOwnerName() const { return m_Owner ? m_Owner->Name : "NONE"; }
};
