#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float time;


float hash(vec3 p) {
    p = fract(p * 0.3183099 + 0.1);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}


float noise(vec3 x) {
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);

    return mix(mix(mix( hash(i + vec3(0,0,0)), hash(i + vec3(1,0,0)), f.x),
                   mix( hash(i + vec3(0,1,0)), hash(i + vec3(1,1,0)), f.x), f.y),
               mix(mix( hash(i + vec3(0,0,1)), hash(i + vec3(1,0,1)), f.x),
                   mix( hash(i + vec3(0,1,1)), hash(i + vec3(1,1,1)), f.x), f.y), f.z);
}

void main() {
    vec3 dir = normalize(TexCoords);

    vec3 distortion = vec3(
        sin(dir.y * 10.0 + time) * 0.05,
        cos(dir.x * 10.0 + time * 1.2) * 0.05,
        sin(dir.z * 10.0 + time * 0.8) * 0.05
    );

    vec3 finalCoords = dir + distortion;

    FragColor = texture(skybox, finalCoords);
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}