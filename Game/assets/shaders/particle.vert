#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord; 

layout(location = 2) in vec3 a_InstancePosition;
layout(location = 3) in vec4 a_InstanceColor;
layout(location = 4) in float a_InstanceSize;
layout(location = 5) in float a_InstanceRotation;

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec4 ParticleColor;

void main()
{
    TexCoord = a_TexCoord;
    ParticleColor = a_InstanceColor;

    // values for rotation
    float c = cos(a_InstanceRotation);
    float s = sin(a_InstanceRotation);

    vec2 rotatedPos;
    rotatedPos.x = a_Position.x * c - a_Position.y * s;
    rotatedPos.y = a_Position.x * s + a_Position.y * c;

    vec3 cameraRight_worldspace = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp_worldspace = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 vertexPosition_worldspace = 
        a_InstancePosition
        + cameraRight_worldspace * rotatedPos.x * a_InstanceSize
        + cameraUp_worldspace    * rotatedPos.y * a_InstanceSize;
    gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0);
}