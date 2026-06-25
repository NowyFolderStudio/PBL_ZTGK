#version 330 core

in vec3 v_LocalPos;
out vec4 FragColor;

uniform vec3 u_RingColor;
uniform float u_Thickness;
uniform float u_Alpha;

void main() {
    float dist = length(v_LocalPos.xz) * 2.0;

    if (dist > 1.0 || dist < (1.0 - u_Thickness)) {
        discard;
    }

    FragColor = vec4(u_RingColor, u_Alpha);

}