#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform float time;

void main() {
    float angle = time * 0.02; 
    mat3 rotY = mat3(
        cos(angle), 0.0, sin(angle),
        0.0,        1.0, 0.0,
        -sin(angle), 0.0, cos(angle)
    );

    TexCoords = rotY * aPos;

    mat4 staticView = mat4(mat3(view));
    vec4 pos = projection * staticView * vec4(aPos, 1.0);
    
    gl_Position = pos.xyww;
}