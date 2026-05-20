#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in ivec4 a_BoneIDs;
layout(location = 4) in vec4 a_Weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    mat4 boneTransform = mat4(0.0);
    bool hasBones = false;

    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if(a_BoneIDs[i] == -1) 
            continue;
            
        hasBones = true;
        boneTransform += finalBonesMatrices[a_BoneIDs[i]] * a_Weights[i];
    }

    if(!hasBones) 
    {
        boneTransform = mat4(1.0);
    }

    vec4 totalPosition = boneTransform * vec4(a_Position, 1.0f);
    gl_Position = projection * view * model * totalPosition;

    // PRZYPISANIE WARTOŚCI (pozycja wierzchołka w świecie)
    FragPos = vec3(model * totalPosition); // <-- TEGO TEŻ BRAKOWAŁO

    TexCoord = a_TexCoord;
    Normal = mat3(transpose(inverse(model * boneTransform))) * a_Normal;
}