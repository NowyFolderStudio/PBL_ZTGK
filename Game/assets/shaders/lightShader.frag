#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture1;

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;

	float constant;
	float linear;
	float quadratic;
};

#define MAX_POINT_LIGHTS 4

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int activePointLights;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    vec3 diffuse = light.color * diff * light.intensity;
    return diffuse * attenuation;
}

void main()
{
    vec3 ambient = vec3(0.1, 0.1, 0.1); 
    
    vec3 norm = normalize(Normal);
    vec3 finalLight = ambient;
    
    for(int i = 0; i < activePointLights; i++) {
        finalLight += CalcPointLight(pointLights[i], norm, FragPos);
    }
    
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = vec4(finalLight, 1.0) * texColor;
}