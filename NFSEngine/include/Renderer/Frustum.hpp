#pragma once

#include <glm/glm.hpp>

namespace NFSEngine {

    struct Plane {
        glm::vec3 Normal = glm::vec3(0.0f);
        float Distance = 0.0f;
    };

    struct BoundingSphere {
        glm::vec3 Center = glm::vec3(0.0f);
        float Radius = 0.0f;
    };

    class Frustum {
    public:
        void ExtractFromMatrix(const glm::mat4& viewProjection);

        bool TestSphere(const BoundingSphere& sphere) const;

        bool TestAABB(const glm::vec3& min, const glm::vec3& max) const;

        const Plane* GetPlanes() const { return m_Planes; }

    private:
        Plane m_Planes[6];
    };

} // namespace NFSEngine
