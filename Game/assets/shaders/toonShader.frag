#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

// Optional for normal mapping
// in mat3 TBN;

out vec4 FragColor;

// Textures and colors
uniform sampler2D u_AlbedoMap;
uniform bool u_HasAlbedoMap;
uniform vec3 u_AlbedoColor;

uniform sampler2D u_RampMap;
uniform bool u_HasRampMap;

// Optional normal map
// uniform sampler2D u_NormalMap;
// uniform bool u_HasNormalMap;

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

float ambientValue    = 0.1;
float specularStrength = 0.1;
float shininess       = 32.0;
float rimThreshold    = 0.75;
float rimAmount       = 0.71;

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

        diffuseTotal += rampColor * radiance;

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

        diffuseTotal += rampColor * radiance;

        vec3 H = normalize(V + L);
        float spec = pow(max(dot(N, H), 0.0), shininess);
        float toonSpec = step(0.5, spec) * specularStrength;
        specularTotal += toonSpec * radiance;
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
}