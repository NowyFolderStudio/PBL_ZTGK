#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture1;

uniform vec3 viewPos;
uniform vec4 u_ColorTint;

struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 4

uniform DirLight dirLight;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int activePointLights;

uniform SpotLight spotLight;

float specularStrength = 0.5;
float shininess = 32.0;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    
    vec3 diffuse = light.color * diff * light.intensity;
    vec3 specular = light.color * spec * specularStrength * light.intensity;
    
    return (diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    vec3 diffuse = light.color * diff * light.intensity;
    vec3 specular = light.color * spec * specularStrength * light.intensity;
    
    return (diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 diffuse = light.color * diff * light.intensity;
    vec3 specular = light.color * spec * specularStrength * light.intensity;
    
    return (diffuse + specular) * attenuation * intensity;
}

void main()
{
    vec3 ambient = vec3(0.05, 0.05, 0.05); 
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 finalLight = ambient;
    
    finalLight += CalcDirLight(dirLight, norm, viewDir);
    
    for(int i = 0; i < activePointLights; i++) {
        finalLight += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    finalLight += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = vec4(finalLight, 1.0) * texColor * u_ColorTint;
}
