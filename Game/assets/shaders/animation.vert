#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout(location = 4) in ivec4 a_BoneIDs;
layout(location = 5) in vec4 a_Weights;

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

    // --- ZABEZPIECZENIE PRZED KOLCAMI I ROZCIĄGANIEM SIATKI ---
    // Upewniamy się, że wagi zawsze, ale to zawsze sumują się do równego 1.0
    float totalWeight = a_Weights.x + a_Weights.y + a_Weights.z + a_Weights.w;
    vec4 normalizedWeights = a_Weights;
    
    if (totalWeight > 0.0) {
        normalizedWeights = a_Weights / totalWeight;
    }

    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        int boneID = int(a_BoneIDs[i]);
        
        // Zmieniamy a_Weights na normalizedWeights
        if(boneID < 0 || boneID >= MAX_BONES || normalizedWeights[i] <= 0.0) 
            continue;
            
        hasBones = true;
        boneTransform += finalBonesMatrices[boneID] * normalizedWeights[i];
    }

    if(!hasBones) 
    {
        boneTransform = mat4(1.0);
    }

    vec4 totalPosition = boneTransform * vec4(a_Position, 1.0f);
    gl_Position = projection * view * model * totalPosition;

    FragPos = vec3(model * totalPosition);
    TexCoord = a_TexCoord;
    Normal = normalize(mat3(model * boneTransform) * a_Normal);
}