#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

uniform float u_MusicTime;
uniform float u_ScaleStrengthY;
uniform float u_ScaleStrengthXZ;
uniform float u_BendStrength;
uniform float u_TwistStrength;

void main()
{
    float distToBeat = abs(fract(u_MusicTime + 0.5) - 0.5);
    float peak = 1.0 - (distToBeat * 2.0);
    float activePhase = max(0.0, (peak - 0.5) * 2.0);
    float wave = pow(activePhase, 2.0);

    vec3 localPos = aPos;

    float waveScaleY = wave * u_ScaleStrengthY;
    localPos.z += waveScaleY * (localPos.z + 1.0); 

    float scaleXZ = 1.0 + (wave * u_ScaleStrengthXZ);
    localPos.x *= scaleXZ;
    localPos.y *= scaleXZ;

    float twistAngle = wave * u_TwistStrength * (localPos.z + 1.0);
    float s = sin(twistAngle);
    float c = cos(twistAngle);

    vec3 twistedPos = localPos;
    twistedPos.x = localPos.x * c - localPos.y * s;
    twistedPos.y = localPos.x * s + localPos.y * c;

    float bendOffset = pow(max(0.0, twistedPos.z + 1.0), 2.0) * wave * u_BendStrength;
    vec3 finalLocalPos = twistedPos + vec3(bendOffset, 0.0, 0.0);

    gl_Position = lightSpaceMatrix * model * vec4(finalLocalPos, 1.0);
}