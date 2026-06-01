#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 u_BBoxMin;
uniform vec3 u_BBoxMax;
uniform float u_MusicTime;
uniform float u_ScaleStrengthY;
uniform float u_ScaleStrengthXZ;
uniform float u_BendStrength;
uniform float u_TwistStrength;

void main() {
    float distToBeat = abs(fract(u_MusicTime + 0.5) - 0.5);
    float peak = 1.0 - (distToBeat * 2.0);
    float activePhase = max(0.0, (peak - 0.5) * 2.0);
    float wave = pow(activePhase, 2.0);

    float height = u_BBoxMax.y - u_BBoxMin.y;
    float halfHeight = height * 0.5;
    vec3 bboxCenter = (u_BBoxMin + u_BBoxMax) * 0.5;

    vec3 localPos = aPos - bboxCenter;

    float normY = (localPos.y + halfHeight) / max(height, 0.001);

    float waveScaleY = wave * u_ScaleStrengthY;
    localPos.y += waveScaleY * (normY * 2.0);

    float scaleXZ = 1.0 + (wave * u_ScaleStrengthXZ);
    localPos.x *= scaleXZ;
    localPos.z *= scaleXZ;

    float twistAngle = wave * u_TwistStrength * (normY * 2.0);
    float s = sin(twistAngle);
    float c = cos(twistAngle);
    vec3 twistedPos;
    twistedPos.x = localPos.x * c - localPos.z * s;
    twistedPos.z = localPos.x * s + localPos.z * c;
    twistedPos.y = localPos.y;

    float bendFactor = normY * 2.0;
    float bendOffset = pow(bendFactor, 2.0) * wave * u_BendStrength;
    vec3 finalLocalPos = twistedPos + vec3(bendOffset, 0.0, 0.0);

    finalLocalPos += bboxCenter;

    FragPos = vec3(model * vec4(finalLocalPos, 1.0));
    TexCoord = aTexCoord;

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    Normal = normalMatrix * aNormal;

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}