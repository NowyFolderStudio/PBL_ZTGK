#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 viewPos;
uniform vec4 u_ColorTint;

struct DirLight {
    vec3 direction; vec3 color; float intensity;
};

struct PointLight {
    vec3 position; vec3 color; float intensity;
    float constant; float linear; float quadratic;
};

struct SpotLight {
    vec3 position; vec3 direction; vec3 color; float intensity;
    float cutOff; float outerCutOff; float constant; float linear; float quadratic;
};

#define MAX_POINT_LIGHTS 4
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int activePointLights;
uniform SpotLight spotLight;

vec3 coolColor = vec3(0.0, 0.0, 0.8);
vec3 warmColor = vec3(0.8, 0.8, 0.0);

vec3 CalcGooch(vec3 lightDir, vec3 lightColor, float intensity, vec3 normal, vec3 viewDir, vec3 texColor) {
    float NdotL = dot(normal, lightDir);
    
    float it = (1.0 + NdotL) * 0.5;

    vec3 diffuse = mix(coolColor, warmColor, it) * texColor;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = lightColor * spec * intensity;

    return (diffuse + specular) * intensity;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(-light.direction);
    return CalcGooch(lightDir, light.color, light.intensity, normal, viewDir, texColor);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    return CalcGooch(lightDir, light.color, light.intensity, normal, viewDir, texColor) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float spotIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    return CalcGooch(lightDir, light.color, light.intensity, normal, viewDir, texColor) * attenuation * spotIntensity;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec4 texel = texture(texture1, TexCoord) * u_ColorTint;
    
    if(texel.a < 0.1) discard;

    vec3 finalLight = vec3(0.0);
    
    finalLight += CalcDirLight(dirLight, norm, viewDir, texel.rgb);
    
    for(int i = 0; i < activePointLights; i++) {
        finalLight += CalcPointLight(pointLights[i], norm, FragPos, viewDir, texel.rgb);
    }

    finalLight += CalcSpotLight(spotLight, norm, FragPos, viewDir, texel.rgb);
    
    FragColor = vec4(finalLight, texel.a);
}