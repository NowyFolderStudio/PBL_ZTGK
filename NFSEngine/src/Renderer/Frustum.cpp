#include "Renderer/Frustum.hpp"
#include <glm/gtc/matrix_access.hpp>

namespace NFSEngine {

    void Frustum::ExtractFromMatrix(const glm::mat4& viewProjection) {

        glm::vec4 rows[4];
        for (int r = 0; r < 4; r++) {
            rows[r] = glm::row(viewProjection, r);
        }

        auto normalizePlane = [](const glm::vec4& p) -> Plane {
            float len = glm::length(glm::vec3(p));
            if (len < 1e-8f) return {glm::vec3(0.0f), 0.0f};
            return {glm::vec3(p) / len, p.w / len};
        };

        m_Planes[0] = normalizePlane(rows[3] + rows[0]); // left
        m_Planes[1] = normalizePlane(rows[3] - rows[0]); // right
        m_Planes[2] = normalizePlane(rows[3] + rows[1]); // bottom
        m_Planes[3] = normalizePlane(rows[3] - rows[1]); // top
        m_Planes[4] = normalizePlane(rows[3] + rows[2]); // near
        m_Planes[5] = normalizePlane(rows[3] - rows[2]); // far
    }

    bool Frustum::TestSphere(const BoundingSphere& sphere) const {
        for (int i = 0; i < 6; i++) {
            float distance = glm::dot(m_Planes[i].Normal, sphere.Center) + m_Planes[i].Distance;
            if (distance + sphere.Radius < 0.0f) {
                return false;
            }
        }
        return true;
    }

    bool Frustum::TestAABB(const glm::vec3& min, const glm::vec3& max) const {
        glm::vec3 center = (min + max) * 0.5f;
        glm::vec3 halfSize = (max - min) * 0.5f;

        for (int i = 0; i < 6; i++) {
            const Plane& plane = m_Planes[i];

            glm::vec3 pVertex = center;
            pVertex.x += (plane.Normal.x >= 0.0f) ? halfSize.x : -halfSize.x;
            pVertex.y += (plane.Normal.y >= 0.0f) ? halfSize.y : -halfSize.y;
            pVertex.z += (plane.Normal.z >= 0.0f) ? halfSize.z : -halfSize.z;

            float distance = glm::dot(plane.Normal, pVertex) + plane.Distance;
            if (distance < 0.0f) {
                return false;
            }
        }
        return true;
    }

} // namespace NFSEngine
