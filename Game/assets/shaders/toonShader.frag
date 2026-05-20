#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D u_AlbedoMap;
uniform bool u_HasAlbedoMap;
uniform vec3 u_AlbedoColor;

uniform sampler2D u_RampMap;
uniform bool u_HasRampMap;

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

#define MAX_POINT_LIGHTS 16
#define MAX_SPOT_LIGHTS 4

uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int activePointLights;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int activeSpotLights;

const float PI = 3.14159265359;

float ambientValue = 0.1;
float specularStrength = 0.1;
float shininess = 8.0;
float rimThreshold = 0.1;
float rimAmount = 0.71;

vec3 GetRampDiffuse(float NdotL) {
    float diffUV = NdotL * 0.5 + 0.5;
    if (u_HasRampMap) {
        return texture(u_RampMap, vec2(diffUV, 0.5)).rgb;
    }
    return diffUV > 0.5 ? vec3(1.0) : vec3(0.3);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(-light.direction);
    vec3 radiance = light.color * light.intensity;

    float NdotL = dot(normal, lightDir);
    vec3 rampDiffuse = GetRampDiffuse(NdotL);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float specIntensity = pow(NdotH, shininess);
    float toonSpec = smoothstep(0.5, 0.5, specIntensity);
    
    float rimDot = 1.0 - dot(viewDir, normal);
    float rimIntensity = rimDot * pow(NdotL, rimThreshold);
    float toonRim = smoothstep(rimAmount - 0.01, rimAmount + 0.01, rimIntensity);
    
    vec3 diffuse = (rampDiffuse / PI) * radiance * albedo;
    vec3 specular = toonSpec * specularStrength * radiance;
    vec3 rim = toonRim * radiance * 0.2;
    
    return (diffuse + specular + rim);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(light.position - fragPos);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / max(distance * distance, 0.0001); 
    
    vec3 radiance = light.color * light.intensity * attenuation;
    
    float NdotL = dot(normal, lightDir);
    vec3 rampDiffuse = GetRampDiffuse(NdotL);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float specIntensity = pow(NdotH, shininess);
    float toonSpec = smoothstep(0.5, 0.5, specIntensity);
    
    vec3 diffuse = (rampDiffuse / PI) * radiance * albedo;
    vec3 specular = toonSpec * specularStrength * radiance;
    
    return (diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / max(distance * distance, 0.0001); 
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float spotIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    

    vec3 radiance = light.color * light.intensity * attenuation * spotIntensity;

    float NdotL = dot(normal, lightDir);
    vec3 rampDiffuse = GetRampDiffuse(NdotL);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float specIntensity = pow(NdotH, shininess);
    float toonSpec = smoothstep(0.5, 0.5, specIntensity);
    
    vec3 diffuse = (rampDiffuse / PI) * radiance * albedo;
    vec3 specular = toonSpec * specularStrength * radiance;
    
    return (diffuse + specular);
}

void main()
{
    vec3 albedo = u_HasAlbedoMap ? pow(texture(u_AlbedoMap, TexCoord).rgb, vec3(2.2)) : u_AlbedoColor;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 finalLight = vec3(ambientValue) * albedo; 
    
    finalLight += CalcDirLight(dirLight, norm, viewDir, albedo);
    
    for(int i = 0; i < activePointLights; i++) {
        finalLight += CalcPointLight(pointLights[i], norm, FragPos, viewDir, albedo);
    }

    for(int i = 0; i < activeSpotLights; i++) {
        finalLight += CalcSpotLight(spotLights[i], norm, FragPos, viewDir, albedo);
    }
    
    FragColor = vec4(finalLight, 1.0);
}