#include "Components/Transform.hpp"
#include "Debug/Profiler.hpp"
#include "imgui.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace NFSEngine {

    void Transform::SetParent(Transform* newParent, bool worldPositionStays) {
        if (newParent == this || newParent == m_Parent) return;

        if (worldPositionStays) {
            glm::mat4 globalMatrix = GetGlobalMatrix();

            if (newParent) {
                glm::mat4 newParentInv = glm::inverse(newParent->GetGlobalMatrix());
                glm::mat4 newLocalMatrix = newParentInv * globalMatrix;

                glm::vec3 skew;
                glm::vec4 perspective;
                glm::decompose(newLocalMatrix, m_Scale, m_Rotation, m_Position, skew, perspective);
            } else {
                glm::vec3 skew;
                glm::vec4 perspective;
                glm::decompose(globalMatrix, m_Scale, m_Rotation, m_Position, skew, perspective);
            }
        }

        if (m_Parent) {
            auto it = std::find(m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this);
            if (it != m_Parent->m_Children.end()) {
                m_Parent->m_Children.erase(it);
            }
        }

        m_Parent = newParent;

        if (m_Parent) {
            m_Parent->m_Children.push_back(this);
        }

        SetDirty();
    }

    void Transform::Move(const glm::vec3& moveVector) {
        glm::vec3 newPosition = GetPosition();
        newPosition += moveVector;
        SetPosition(newPosition);
    }

    void Transform::Rotate(const glm::vec3& rotationVector) {
        glm::vec3 radians = glm::radians(rotationVector);
        glm::quat rotationOffset = glm::quat(radians);
        m_Rotation = m_Rotation * rotationOffset;
        m_Rotation = glm::normalize(m_Rotation);
        SetDirty();
    }

    void Transform::Scale(const glm::vec3& scaleVector) {
        glm::vec3 newScale = GetScale();
        newScale *= scaleVector;
        SetScale(newScale);
    }

    void Transform::SetDirty() {
        if (!m_Dirty) {
            m_Dirty = true;
            for (auto* child : m_Children) {
                child->SetDirty();
            }
        }
    }

    void Transform::RecalculateMatrix() {
        NFS_PROFILE_FUNCTION();
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);
        glm::mat4 rotation = glm::mat4_cast(m_Rotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

        m_LocalMatrix = translation * rotation * scale;

        if (m_Parent) {
            m_GlobalMatrix = m_Parent->GetGlobalMatrix() * m_LocalMatrix;
        } else {
            m_GlobalMatrix = m_LocalMatrix;
        }

        m_Dirty = false;
    }

    glm::vec3 Transform::GetForward() { return glm::normalize(glm::vec3(GetGlobalMatrix()[2])); }

    glm::vec3 Transform::GetUp() { return glm::normalize(glm::vec3(GetGlobalMatrix()[1])); }

    glm::vec3 Transform::GetWorldPosition() { return glm::vec3(GetGlobalMatrix()[3]); }

    glm::vec3 Transform::GetWorldScale() {
        const glm::mat4& mat = GetGlobalMatrix();
        return glm::vec3(glm::length(glm::vec3(mat[0])), glm::length(glm::vec3(mat[1])), glm::length(glm::vec3(mat[2])));
    }

    glm::quat Transform::GetWorldRotation() {
        const glm::mat4& mat = GetGlobalMatrix();

        glm::vec3 right = glm::normalize(glm::vec3(mat[0]));
        glm::vec3 up = glm::normalize(glm::vec3(mat[1]));
        glm::vec3 forward = glm::normalize(glm::vec3(mat[2]));

        return glm::quat_cast(glm::mat3(right, up, forward));
    }

    void Transform::OnImGuiRender() {
        glm::vec3 pos = GetPosition();
        if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.1f)) {
            SetPosition(pos);
        }

        glm::vec3 rotation = glm::degrees(glm::eulerAngles(GetRotation()));
        if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f)) {
            SetRotation(rotation); // Twoja metoda konwertująca stopnie na kwaternion
        }

        glm::vec3 scale = GetScale();
        if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f)) {
            SetScale(scale);
        }
    }
} // namespace NFSEngine