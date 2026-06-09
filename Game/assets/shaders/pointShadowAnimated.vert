#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 4) in ivec4 a_BoneIDs;
layout(location = 5) in vec4 a_Weights;

uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    mat4 boneTransform = mat4(0.0);
    float weightSum = 0.0;

    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(a_BoneIDs[i] == -1) 
            continue;
        if(a_BoneIDs[i] >= MAX_BONES) 
            break;

        float w = a_Weights[i];
        boneTransform += finalBonesMatrices[a_BoneIDs[i]] * w;
        weightSum += w;
    }

    if(weightSum > 0.0)
        boneTransform /= weightSum;
    else
        boneTransform = mat4(1.0);

    gl_Position = model * boneTransform * vec4(a_Position, 1.0);
}