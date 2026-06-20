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

    vec3 localPos = aPos;

    // Skalowanie (Twoje poprawki dla osi Z jako wysokości)
    float waveScaleY = wave * u_ScaleStrengthY;
    localPos.z += waveScaleY * (localPos.z + 1.0); 

    float scaleXZ = 1.0 + (wave * u_ScaleStrengthXZ);
    localPos.x *= scaleXZ;
    localPos.y *= scaleXZ;

    // --- POPRAWIONY TWIST ---
    // Siła skrętu zależy teraz od osi Z (wysokości)
    float twistAngle = wave * u_TwistStrength * (localPos.z + 1.0);
    float s = sin(twistAngle);
    float c = cos(twistAngle);

    vec3 twistedPos = localPos;
    // Obracamy płaszczyznę poziimą (X oraz Y) wokół osi Z
    twistedPos.x = localPos.x * c - localPos.y * s;
    twistedPos.y = localPos.x * s + localPos.y * c;

    // --- POPRAWIONY BEND ---
    // Siła wygięcia rośnie wraz z osią Z (wysokością)
    float bendOffset = pow(max(0.0, twistedPos.z + 1.0), 2.0) * wave * u_BendStrength;
    // Przesuwamy model wzdłuż osi X
    vec3 finalLocalPos = twistedPos + vec3(bendOffset, 0.0, 0.0);

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