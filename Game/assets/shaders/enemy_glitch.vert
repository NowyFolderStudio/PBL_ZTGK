#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float u_Time;
uniform float u_Aggro; 

void main() {
    TexCoord = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;

    float frequency = 4.0; 
    float amplitude = 0.15 + (u_Aggro * 0.3);

    float displacement = sin(aPos.x * frequency + u_Time) *
                         sin(aPos.y * frequency + u_Time) *
                         sin(aPos.z * frequency + u_Time);

    vec3 displacedPos = aPos + (aNormal * displacement * amplitude);

    FragPos = vec3(model * vec4(displacedPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}