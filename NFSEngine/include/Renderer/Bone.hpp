#include "Core/Assert.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/common.hpp>
#include "nfspch.h"

namespace NFSEngine {

    struct KeyPosition {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyRotation {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyScale {
        glm::vec3 scale;
        float timeStamp;
    };

    class Bone {
    private:
        std::vector<KeyPosition> m_Positions;
        std::vector<KeyRotation> m_Rotations;
        std::vector<KeyScale> m_Scales;
        int m_NumPositions;
        int m_NumRotations;
        int m_NumScalings;

        glm::mat4 m_LocalTransform;
        std::string m_Name;
        int m_ID;

    public:
        Bone(const std::string& name, int ID, const aiNodeAnim* channel)
            : m_Name(name)
            , m_ID(ID)
            , m_LocalTransform(1.0f) {
            m_NumPositions = channel->mNumPositionKeys;

            m_NumPositions = channel->mNumPositionKeys;

            for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex) {
                aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
                float timeStamp = channel->mPositionKeys[positionIndex].mTime;
                KeyPosition data;
                data.position = GetGLMVec(aiPosition);
                data.timeStamp = timeStamp;
                m_Positions.push_back(data);
            }

            m_NumRotations = channel->mNumRotationKeys;
            for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex) {
                aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
                float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
                KeyRotation data;
                data.orientation = GetGLMQuat(aiOrientation);
                data.timeStamp = timeStamp;
                m_Rotations.push_back(data);
            }

            m_NumScalings = channel->mNumScalingKeys;
            for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex) {
                aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
                float timeStamp = channel->mScalingKeys[keyIndex].mTime;
                KeyScale data;
                data.scale = GetGLMVec(scale);
                data.timeStamp = timeStamp;
                m_Scales.push_back(data);
            }
        }

        void Update(float animationTime) {
            float maxTime = 0.0f;
            if (!m_Positions.empty()) maxTime = std::max(maxTime, m_Positions.back().timeStamp);
            if (!m_Rotations.empty()) maxTime = std::max(maxTime, m_Rotations.back().timeStamp);
            if (!m_Scales.empty()) maxTime = std::max(maxTime, m_Scales.back().timeStamp);
            if (animationTime > maxTime) animationTime = maxTime;

            glm::mat4 translation = InterpolatePosition(animationTime);
            glm::mat4 rotation = InterpolateRotation(animationTime);
            glm::mat4 scale = InterpolateScaling(animationTime);
            m_LocalTransform = translation * rotation * scale;
        }

        glm::mat4 GetLocalTransform() { return m_LocalTransform; }
        std::string GetBoneName() const { return m_Name; }
        int GetBoneID() { return m_ID; }

        /* Gets the current index on mKeyPositions to interpolate to based on
    the current animation time*/
        int GetPositionIndex(float animationTime) {
            for (int index = 0; index < m_NumPositions - 1; ++index) {
                if (animationTime < m_Positions[index + 1].timeStamp) return index;
            }
            return m_NumPositions - 2; // ostatni przedział
        }

        /* Gets the current index on mKeyRotations to interpolate to based on the
        current animation time*/
        int GetRotationIndex(float animationTime) {
            for (int index = 0; index < m_NumRotations - 1; ++index) {
                if (animationTime < m_Rotations[index + 1].timeStamp) return index;
            }
            return m_NumRotations - 2;
        }

        /* Gets the current index on mKeyScalings to interpolate to based on the
        current animation time */
        int GetScaleIndex(float animationTime) {
            for (int index = 0; index < m_NumScalings - 1; ++index) {
                if (animationTime < m_Scales[index + 1].timeStamp) return index;
            }
            return m_NumScalings - 2;
        }

        glm::vec3 GetLocalPosition(float animationTime) {
            if (m_Positions.empty()) return glm::vec3(0.0f);
            if (1 == m_NumPositions) return m_Positions[0].position;

            int p0Index = GetPositionIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
            return glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
        }

        glm::quat GetLocalRotation(float animationTime) {
            if (m_Rotations.empty()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            if (1 == m_NumRotations) return glm::normalize(m_Rotations[0].orientation);

            int p0Index = GetRotationIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
            glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
            return glm::normalize(finalRotation);
        }

        glm::vec3 GetLocalScale(float animationTime) {
            if (m_Scales.empty()) return glm::vec3(1.0f);
            if (1 == m_NumScalings) return m_Scales[0].scale;

            int p0Index = GetScaleIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
            return glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
        }

    private:
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
            float scaleFactor = 0.0f;
            float midWayLength = animationTime - lastTimeStamp;
            float framesDiff = nextTimeStamp - lastTimeStamp;
            scaleFactor = midWayLength / framesDiff;
            return scaleFactor;
        }

        /*figures out which position keys to interpolate b/w and performs the interpolation
        and returns the translation matrix*/
        glm::mat4 InterpolatePosition(float animationTime) {
            if (1 == m_NumPositions) return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

            int p0Index = GetPositionIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
            glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
            return glm::translate(glm::mat4(1.0f), finalPosition);
        }

        /*figures out which rotations keys to interpolate b/w and performs the interpolation
        and returns the rotation matrix*/
        glm::mat4 InterpolateRotation(float animationTime) {
            if (1 == m_NumRotations) {
                auto rotation = glm::normalize(m_Rotations[0].orientation);
                return glm::toMat4(rotation);
            }

            int p0Index = GetRotationIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
            glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
            finalRotation = glm::normalize(finalRotation);
            return glm::toMat4(finalRotation);
        }

        /*figures out which scaling keys to interpolate b/w and performs the interpolation
        and returns the scale matrix*/
        glm::mat4 InterpolateScaling(float animationTime) {
            if (1 == m_NumScalings) return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

            int p0Index = GetScaleIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
            glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
            return glm::scale(glm::mat4(1.0f), finalScale);
        }

        static inline glm::vec3 GetGLMVec(const aiVector3D& vec) { return glm::vec3(vec.x, vec.y, vec.z); }
        static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation) {
            return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
        }
    };

} // namespace NFSEngine