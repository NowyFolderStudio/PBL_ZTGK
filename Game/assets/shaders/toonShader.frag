#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D rampTexture;

uniform vec3 viewPos;

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

float specularSize = 0.1;
float rimThreshold = 0.1;
float rimAmount = 0.71;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // 1. DIFFUSE RAMP
    float NdotL = dot(normal, lightDir);
    float diffUV = NdotL * 0.5 + 0.5;
    vec3 rampDiffuse = texture(rampTexture, vec2(diffUV, 0.5)).rgb;

    // 2. TOON SPECULAR
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float specIntensity = pow(NdotH, 128.0);
    float toonSpec = smoothstep(0.5 - specularSize, 0.5, specIntensity);
    
    // 3. RIM LIGHTING
    float rimDot = 1.0 - dot(viewDir, normal);
    float rimIntensity = rimDot * pow(NdotL, rimThreshold);
    float toonRim = smoothstep(rimAmount - 0.01, rimAmount + 0.01, rimIntensity);
    
    vec3 diffuse = light.color * rampDiffuse * light.intensity;
    vec3 specular = light.color * toonSpec * light.intensity;
    vec3 rim = light.color * toonRim * light.intensity;
    
    return (diffuse + specular + rim);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 1. DIFFUSE RAMP
    float NdotL = dot(normal, lightDir);
    float diffUV = NdotL * 0.5 + 0.5; // Mapowanie na [0, 1] dla tekstury
    vec3 rampDiffuse = texture(rampTexture, vec2(diffUV, 0.5)).rgb;
    
    // 2. TOON SPECULAR (Ostre bliki)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float specIntensity = pow(NdotH, 128.0);
    float toonSpec = smoothstep(0.5 - specularSize, 0.5, specIntensity);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    vec3 diffuse = light.color * rampDiffuse * light.intensity;
    vec3 specular = light.color * toonSpec * light.intensity;
    
    return (diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 1. STOŻEK REFLEKTORA (Intensity)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float spotIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    

    // 2. DIFFUSE RAMP
    float NdotL = dot(normal, lightDir);
    float diffUV = NdotL * 0.5 + 0.5;
    vec3 rampDiffuse = texture(rampTexture, vec2(diffUV, 0.5)).rgb;
    
    // 3. TOON SPECULAR
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float specIntensity = pow(NdotH, 128.0);
    float toonSpec = smoothstep(0.5 - specularSize, 0.5, specIntensity);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 diffuse = light.color * rampDiffuse * light.intensity;
    vec3 specular = light.color * toonSpec * light.intensity;
    
    return (diffuse + specular) * attenuation * spotIntensity;
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
    FragColor = vec4(finalLight, 1.0) * texColor;
}