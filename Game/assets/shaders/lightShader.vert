#version 330 core
layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// for shadow mapping
out vec4 FragPosLightSpace;
uniform mat4 lightSpaceMatrix;

void main() {
	gl_Position = projection * view * model * vec4(a_Pos, 1.0);
    FragPos = vec3(model * vec4(a_Pos, 1.0));
    TexCoord = a_TexCoord;

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    Normal = normalMatrix * a_Normal;

    vec3 T = normalize(normalMatrix * a_Tangent);
    vec3 N = normalize(normalMatrix * a_Normal);
    
    T = normalize(T - dot(T, N) * N);
    
    vec3 B = cross(N, T);
    
    TBN = mat3(T, B, N);

    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}