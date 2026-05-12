#pragma once
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>

namespace NFSEngine {

    using Vector3 = glm::vec3;
    using Vector2 = glm::vec2;
    using Vector4 = glm::vec4;
    using Quaternion = glm::quat;

    class Math {
    public:
        static float MoveTowards(float current, float target, float maxDelta) {
            if (std::abs(target - current) <= maxDelta) {
                return target;
            }
            return current + ((target > current) ? maxDelta : -maxDelta);
        }

        static float Lerp(float a, float b, float t) {
            t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
            return a + t * (b - a);
        }

        static float Clamp(float value, float min, float max) {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        static float DeltaAngle(float current, float target) {
            float delta = std::fmod(target - current, 360.0f);
            if (delta > 180.0f)
                delta -= 360.0f;
            else if (delta < -180.0f)
                delta += 360.0f;
            return delta;
        }

        static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float deltaTime,
                                float maxSpeed = INFINITY) {
            smoothTime = std::max(0.0001f, smoothTime);
            float omega = 2.0f / smoothTime;
            float x = omega * deltaTime;
            float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
            float change = current - target;
            float originalTo = target;

            float maxChange = maxSpeed * smoothTime;
            change = Math::Clamp(change, -maxChange, maxChange);
            target = current - change;

            float temp = (currentVelocity + omega * change) * deltaTime;
            currentVelocity = (currentVelocity - omega * temp) * exp;
            float output = target + (change + temp) * exp;

            if ((originalTo - current > 0.0f) == (output > originalTo)) {
                output = originalTo;
                currentVelocity = (output - originalTo) / deltaTime;
            }
            return output;
        }

        static float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float deltaTime,
                                     float maxSpeed = INFINITY) {
            target = current + DeltaAngle(current, target);
            return SmoothDamp(current, target, currentVelocity, smoothTime, deltaTime, maxSpeed);
        }
    };

} // namespace NFSEngine