#pragma once

#include <string>
#include <ostream>
#include "Core/DeltaTime.hpp"

namespace NFSEngine
{
class GameObject;

class Component
{
public:
    virtual ~Component() = default;

    void Awake();
    void Start();
    void FixedUpdate(DeltaTime fixedDeltaTime);
    void Update(DeltaTime deltaTime);
    void Render();

    bool IsActive() const { return m_Active; }
    void SetActive(bool isActive);

    std::string GetName() const { return m_Name; }
    std::string GetOwnerName() const;

    // This make class able to be logged
    friend std::ostream& operator<<(std::ostream& os, const Component& component)
    {
        return os << component.GetName() << " of " << component.GetOwnerName() << " GameObject";
    }

protected:
    Component(GameObject* owner)
        : m_Owner(owner)
    {
    }
    GameObject* m_Owner = nullptr;
    std::string m_Name = "GameObject";

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
