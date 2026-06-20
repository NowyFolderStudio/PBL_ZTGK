#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
out vec4 FragPosLightSpace;

uniform float u_Time;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main() {
    float CFG_GlitchChance = 0.6;
    float CFG_GlitchIntensity = 0.4;
    
    float CFG_BounceSpeed = 5.0;
    float CFG_BounceHeight = 0.6;
    float CFG_SquashStretchAmount = 0.25;

    vec3 pos = a_Pos;

    float bounceWave = sin(u_Time * CFG_BounceSpeed);
    
    float yScale = 1.0;
    float xzScale = 1.0;
    float hopOffset = 0.0;

    if (bounceWave > 0.0) {
        yScale = 1.0 + (bounceWave * CFG_SquashStretchAmount);
        xzScale = 1.0 - (bounceWave * CFG_SquashStretchAmount * 0.5);
        hopOffset = bounceWave * CFG_BounceHeight;
    } else {
        yScale = 1.0 + (bounceWave * CFG_SquashStretchAmount * 1.5);
        xzScale = 1.0 - (bounceWave * CFG_SquashStretchAmount * 0.7);
        hopOffset = 0.0;
    }

    pos.y = pos.y * yScale;
    
    pos.x *= xzScale;
    pos.z *= xzScale;

    pos.y += hopOffset;


    float glitchRoll = random(vec2(u_Time, 0.0));
    if (glitchRoll > CFG_GlitchChance) {
        float glitchOffsetX = (random(vec2(u_Time, pos.y)) - 0.5) * 2.0;
        float glitchOffsetZ = (random(vec2(u_Time + 42.0, pos.y)) - 0.5) * 2.0;
        
        pos.x += glitchOffsetX * CFG_GlitchIntensity; 
        pos.z += glitchOffsetZ * CFG_GlitchIntensity; 
    }


    gl_Position = projection * view * model * vec4(pos, 1.0);
    FragPos = vec3(model * vec4(pos, 1.0));
    TexCoord = a_TexCoord;

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    Normal = normalMatrix * a_Normal;

    vec3 T = normalize(normalMatrix * a_Tangent);
    vec3 N = normalize(normalMatrix * a_Normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}