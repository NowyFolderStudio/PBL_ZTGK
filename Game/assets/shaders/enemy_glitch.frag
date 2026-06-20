#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 OutlineColor;
layout (location = 3) out vec4 OutNormal;
layout (location = 4) out vec4 OutOutlineParams;

in vec2 TexCoord;
in vec3 Normal;

uniform float u_Time;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main() {
    float CFG_NoiseScale = 350.0;
    float CFG_NoiseSpeed = 15.0;
    
    float CFG_ScanlineDensity = 20.0;
    float CFG_ScanlineSpeed = 10.0;
    float CFG_ScanlineDarkness = 0.15;

    float CFG_RollBandDensity = 3.0;
    float CFG_RollBandSpeed = 2.0;
    
    vec3 CFG_TvTint = vec3(0.85, 0.90, 0.95);

    vec2 noiseUV = floor(TexCoord * CFG_NoiseScale) / CFG_NoiseScale;
    
    float noise = random(noiseUV + fract(u_Time * CFG_NoiseSpeed));
    vec3 baseColor = vec3(noise * 0.7 + 0.15); 

    float scanline = sin(TexCoord.y * CFG_ScanlineDensity - u_Time * CFG_ScanlineSpeed);
    baseColor -= (scanline * 0.5 + 0.5) * CFG_ScanlineDarkness;

    float rollBand = sin(TexCoord.y * CFG_RollBandDensity - u_Time * CFG_RollBandSpeed);
    baseColor += (rollBand * 0.5 + 0.5) * 0.08;

    float vignette = distance(TexCoord, vec2(0.5));

    baseColor *= smoothstep(0.8, 0.3, vignette);

    baseColor *= CFG_TvTint;

    FragColor = vec4(baseColor, 1.0); 
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0); 
    OutNormal = vec4(normalize(Normal), 1.0); 
    OutlineColor = vec4(0.0, 0.0, 0.0, 1.0); 
    OutOutlineParams = vec4(0.08, 1.1, 0.30, 1.0);
}