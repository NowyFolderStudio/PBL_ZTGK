#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Components/Component.hpp"

namespace NFSEngine {
    class Transform : public Component {
    public:
        Transform(GameObject* owner)
            : Component(owner) { }

        std::string GetName() const override { return "Transform"; }

        // SCENE GRAPH
        void SetParent(Transform* newParent, bool worldPositionStays = true);

        Transform* GetParent() { return m_Parent; }
        int GetChildCount() { return static_cast<int>(m_Children.size()); }
        Transform* GetChild(int index) { return m_Children[index]; }

        // TRANSFORMATION MATRIX
        void SetPosition(const glm::vec3& position) {
            m_Position = position;
            SetDirty();
        }

        void SetRotation(const glm::vec3& eulerDegrees) {
            m_Rotation = glm::quat(glm::radians(eulerDegrees));
            SetDirty();
        }

        void SetScale(const glm::vec3& scale) {
            m_Scale = scale;
            SetDirty();
        }

        const glm::vec3& GetPosition() const { return m_Position; }
        const glm::quat& GetRotation() const { return m_Rotation; }
        const glm::vec3& GetScale() const { return m_Scale; }
        const glm::mat4& GetGlobalMatrix() {
            if (m_Dirty) {
                RecalculateMatrix();
            }
            return m_GlobalMatrix;
        }

        void Move(const glm::vec3& moveVector);
        void Rotate(const glm::vec3& rotationVector);
        void Scale(const glm::vec3& scaleVector);

        // ImGui

        void OnImGuiRender() override;

    private:
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        glm::quat m_Rotation = glm::quat { 1.0f, 0.0f, 0.0f, 0.0f };
        glm::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };

        glm::mat4 m_LocalMatrix = glm::mat4(1.0f);
        glm::mat4 m_GlobalMatrix = glm::mat4(1.0f);
        bool m_Dirty = true;

        // Scene graph
        Transform* m_Parent = nullptr;
        std::vector<Transform*> m_Children;

        void SetDirty();

        void RecalculateMatrix();
    };

} // namespace NFSEngine