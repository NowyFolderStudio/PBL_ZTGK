#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
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
out mat3 TBN;

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

    vec4 localPosition = boneTransform * vec4(a_Position, 1.0);
    vec4 worldPosition = model * localPosition;

    gl_Position = projection * view * worldPosition;
    FragPos = vec3(worldPosition);
    TexCoord = a_TexCoord;

    mat3 worldMatrix3 = mat3(model * boneTransform);
    vec3 col0 = normalize(worldMatrix3[0]);
    vec3 col1 = worldMatrix3[1] - dot(col0, worldMatrix3[1]) * col0;
    col1 = normalize(col1);
    vec3 col2 = cross(col0, col1);
    mat3 rotMatrix = mat3(col0, col1, col2);

    vec3 N = normalize(rotMatrix * a_Normal);
    vec3 T = normalize(rotMatrix * a_Tangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    Normal = N;
    TBN = mat3(T, B, N);
}