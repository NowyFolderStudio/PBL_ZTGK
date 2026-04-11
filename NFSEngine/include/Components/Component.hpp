#pragma once

#include "Core/DeltaTime.hpp"

namespace NFSEngine
{

class GameObject;

class Component
{
public:
    virtual ~Component() = default;

    void Awake()
    {
        if (m_Awakened) return;

        OnAwake();
        m_Awakened = true;
    }

    void Start()
    {
        if (m_Started || !m_Active) return;

        OnStart();
        m_Started = true;
    }

    void FixedUpdate(DeltaTime fixedDeltaTime)
    {
        if (!m_Active) return;

        if (!m_Started)
        {
            Start();
        }
        OnFixedUpdate(fixedDeltaTime);
    }

    void Update(DeltaTime deltaTime)
    {
        if (!m_Active) return;
        if (!m_Started)
        {
            Start();
        }
        OnUpdate(deltaTime);
    }

    void Render()
    {
        if (!m_Active) return;
        OnRender();
    }

    bool IsActive() { return m_Active; }

    void SetActive(bool isActive)
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

protected:
    Component(GameObject* owner)
        : p_Owner(owner)
    {
    }
    GameObject* p_Owner = nullptr;

    bool m_Awakened = false;
    bool m_Started = false;
    bool m_Active = true;

    // Use for initialization calls within an object
    virtual void OnAwake() { }
    // Use for initialization calls that may depend on other objects
    virtual void OnStart() { }
    virtual void OnFixedUpdate(DeltaTime fixedDeltaTime) { }
    virtual void OnUpdate(DeltaTime deltaTime) { }
    virtual void OnRender() { }

    // Used when we change component active flag from false to true
    virtual void OnEnable() { }

    // Used when we change component active flag from true to false
    virtual void OnDisable() { }
};

}
