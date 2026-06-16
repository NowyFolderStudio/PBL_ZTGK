#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

// Optional for normal mapping
// in mat3 TBN;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 OutlineColor;
layout (location = 3) out vec4 OutNormal;
layout (location = 4) out vec4 OutOutlineParams;

// Textures and colors
uniform sampler2D u_AlbedoMap;
uniform bool u_HasAlbedoMap;
uniform vec3 u_AlbedoColor;

uniform sampler2D u_RampMap;
uniform bool u_HasRampMap;

uniform vec3 u_OutlineColor = vec3(0, 0, 0);
uniform float u_OutlineSize = 0.08;
uniform float u_OutlineDepthThreshold = 1.1;
uniform float u_OutlineNormalThreshold = 0.30;

// Optional normal map
// uniform sampler2D u_NormalMap;
// uniform bool u_HasNormalMap;

uniform vec3 viewPos;

// shadow things
in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;

uniform samplerCube pointShadowMap;
uniform float pointShadowFarPlane;

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

float ambientValue    = 0.1;
float specularStrength = 0.1;
float shininess       = 32.0;
float rimThreshold    = 0.75;
float rimAmount       = 0.71;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float currentDepth = projCoords.z;
    
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    return shadow;
}

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float PointShadowCalculation(vec3 fragPos, vec3 lightPos)
{
    vec3 fragToLight = fragPos - lightPos;
    
    float currentDepth = length(fragToLight);

    if(currentDepth >= pointShadowFarPlane)
        return 0.0;
    
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / pointShadowFarPlane)) / 25.0;
    
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        
        closestDepth *= pointShadowFarPlane;
        
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    return shadow;
}

void main()
{
    vec3 albedo = u_HasAlbedoMap ? texture(u_AlbedoMap, TexCoord).rgb : u_AlbedoColor;

    vec3 N = normalize(Normal);
    /*
    if (u_HasNormalMap) {
        vec3 normalMap = texture(u_NormalMap, TexCoord).rgb * 2.0 - 1.0;
        N = normalize(TBN * normalMap);
    }
    */

    vec3 V = normalize(viewPos - FragPos);

    vec3 diffuseTotal  = vec3(0.0);
    vec3 specularTotal = vec3(0.0);

    // --- Directional light ---
    {
        vec3 L = normalize(-dirLight.direction);
        vec3 radiance = dirLight.color * dirLight.intensity;

        float NdotL = max(dot(N, L), 0.0);

        vec3 rampColor = u_HasRampMap
            ? texture(u_RampMap, vec2(NdotL, 0.5)).rgb
            : vec3(step(0.5, NdotL));

        float shadow = ShadowCalculation(FragPosLightSpace, N, L);
        diffuseTotal += rampColor * radiance * (1.0 - shadow);

        vec3 H = normalize(V + L);
        float spec = pow(max(dot(N, H), 0.0), shininess);
        float specThreshold = 0.5;
        float toonSpec = step(specThreshold, spec) * specularStrength;
        specularTotal += toonSpec * radiance;
    }

    // --- Point lights ---
    for (int i = 0; i < activePointLights; ++i)
    {
        vec3 L = normalize(pointLights[i].position - FragPos);
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance);   // same as friend's PBR
        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;

        float NdotL = max(dot(N, L), 0.0);

        vec3 rampColor = u_HasRampMap
            ? texture(u_RampMap, vec2(NdotL, 0.5)).rgb
            : vec3(step(0.5, NdotL));

        vec3 H = normalize(V + L);
        float spec = pow(max(dot(N, H), 0.0), shininess);
        float toonSpec = step(0.5, spec) * specularStrength;
        
        float shadow = 0.0;
        if(i == 0) {
            shadow = PointShadowCalculation(FragPos, pointLights[i].position);
        }

        diffuseTotal += rampColor * radiance * (1.0 - shadow);
        specularTotal += toonSpec * radiance * (1.0 - shadow);
    }

    // --- Spot lights ---
    for (int i = 0; i < activeSpotLights; ++i)
    {
        vec3 L = normalize(spotLights[i].position - FragPos);
        float distance = length(spotLights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance);

        float theta = dot(L, normalize(-spotLights[i].direction));
        float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
        float spotIntensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

        vec3 radiance = spotLights[i].color * spotLights[i].intensity * attenuation * spotIntensity;

        float NdotL = max(dot(N, L), 0.0);

        vec3 rampColor = u_HasRampMap
            ? texture(u_RampMap, vec2(NdotL, 0.5)).rgb
            : vec3(step(0.5, NdotL));

        diffuseTotal += rampColor * radiance;

        vec3 H = normalize(V + L);
        float spec = pow(max(dot(N, H), 0.0), shininess);
        float toonSpec = step(0.5, spec) * specularStrength;
        specularTotal += toonSpec * radiance;
    }

    // --- Ambient ---
    vec3 ambient = ambientValue * albedo;

    // // --- Rim light ---
    // float rim = 1.0 - max(dot(N, V), 0.0);
    // rim = smoothstep(rimThreshold, rimThreshold, rim);  
    
    // vec3 rimLight = rim * rimAmount * albedo;

    // --- Combine ---
    vec3 finalColor = ambient + albedo * diffuseTotal + specularTotal;

    FragColor = vec4(finalColor, 1.0);
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    OutNormal = vec4(normalize(Normal), 1.0);
    OutlineColor = vec4(u_OutlineColor, 1);
    OutOutlineParams = vec4(u_OutlineSize, u_OutlineDepthThreshold, u_OutlineNormalThreshold, 1.0);
}