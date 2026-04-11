#include "Core/Components/Transform.hpp"

namespace NFSEngine
{

void Transform::AddChild(Transform* child)
{
    child->m_Parent = this;
    m_Children.push_back(child);
    child->SetDirty();
}

void Transform::Move(const glm::vec3& moveVector)
{
    glm::vec3 newPosition = GetPosition();
    newPosition += moveVector;
    SetPosition(newPosition);
}

void Transform::Rotate(const glm::vec3& rotationVector)
{
    glm::vec3 radians = glm::radians(rotationVector);
    glm::quat rotationOffset = glm::quat(radians);
    m_Rotation = m_Rotation * rotationOffset;
    m_Rotation = glm::normalize(m_Rotation);
    SetDirty();
}

void Transform::Scale(const glm::vec3& scaleVector)
{
    glm::vec3 newScale = GetScale();
    newScale *= scaleVector;
    SetScale(newScale);
}

void Transform::SetDirty()
{
    if (!m_Dirty)
    {
        m_Dirty = true;
        for (auto* child : m_Children)
        {
            child->SetDirty();
        }
    }
}

void Transform::RecalculateMatrix()
{
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);
    glm::mat4 rotation = glm::mat4_cast(m_Rotation);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

    m_LocalMatrix = translation * rotation * scale;

    if (m_Parent)
    {
        m_GlobalMatrix = m_Parent->GetGlobalMatrix() * m_LocalMatrix;
    }
    else
    {
        m_GlobalMatrix = m_LocalMatrix;
    }

    m_Dirty = false;
}
}