#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 OutlineColor;
layout (location = 3) out vec4 OutNormal;
layout (location = 4) out vec4 OutOutlineParams;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

in mat3 TBN;

in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;

uniform samplerCube pointShadowMap;
uniform float pointShadowFarPlane;

uniform vec3 viewPos;

uniform sampler2D u_AlbedoMap;
uniform bool u_HasAlbedoMap;
uniform vec3 u_AlbedoColor;

uniform sampler2D u_NormalMap;
uniform bool u_HasNormalMap;

uniform sampler2D u_MetallicMap;
uniform bool u_HasMetallicMap;
uniform float u_Metallic;

uniform sampler2D u_RoughnessMap;
uniform bool u_HasRoughnessMap;
uniform float u_Roughness;

uniform sampler2D u_AOMap;
uniform bool u_HasAOMap;

uniform sampler2D u_EmissiveMap;
uniform bool u_HasEmissiveMap;
uniform vec3 u_EmissiveColor;
uniform float u_EmissiveStrength;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform vec3 u_OutlineColor = vec3(0, 0, 0);
uniform float u_OutlineSize = 0.08;
uniform float u_OutlineDepthThreshold = 1.1;
uniform float u_OutlineNormalThreshold = 0.30;

const float PI = 3.14159265359;

#define MAX_POINT_LIGHTS 16
#define MAX_SPOT_LIGHTS 4

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
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

uniform DirLight dirLight;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int activePointLights;

uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int activeSpotLights;

uniform float u_Time;
uniform float u_DissolveAmount;

float hash(vec3 p) {
    p  = fract( p * 0.3183099 + 0.1 );
    p *= 17.0;
    return fract( p.x * p.y * p.z * (p.x + p.y + p.z) );
}

float noise(vec3 x) {
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix(hash(i + vec3(0,0,0)), hash(i + vec3(1,0,0)), f.x),
                   mix(hash(i + vec3(0,1,0)), hash(i + vec3(1,1,0)), f.x), f.y),
               mix(mix(hash(i + vec3(0,0,1)), hash(i + vec3(1,0,1)), f.x),
                   mix(hash(i + vec3(0,1,1)), hash(i + vec3(1,1,1)), f.x), f.y), f.z);
}

// Normal Distributio Function 
float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / max(denom, 0.0000001);
}

// Geometry Function 
float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Frensel Equation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Frensel 
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

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

void main() {
    float noiseVal = noise(FragPos * 4.0); 
    
    if(noiseVal < u_DissolveAmount) {
        discard; 
    }

	vec3 albedo = u_HasAlbedoMap ? texture(u_AlbedoMap, TexCoord).rgb : u_AlbedoColor;

	vec3 N = normalize(Normal);

    if (u_HasNormalMap) {
        vec3 normalMap = texture(u_NormalMap, TexCoord).rgb;
        normalMap = normalMap * 2.0 - 1.0;
        
        N = normalize(TBN * normalMap);
    }

	float metallic = u_HasMetallicMap ? texture(u_MetallicMap, TexCoord).r : u_Metallic;
    float roughness = u_HasRoughnessMap ? texture(u_RoughnessMap, TexCoord).r : u_Roughness;
    float ao = u_HasAOMap ? texture(u_AOMap, TexCoord).r : 1.0;

	vec3 V = normalize(viewPos - FragPos);

	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);

    // Directional light
 
    vec3 L_dir = normalize(-dirLight.direction);
    vec3 H_dir = normalize(V + L_dir);
    vec3 radiance_dir = dirLight.color * dirLight.intensity;

    float NDF_dir = DistributionGGX(N, H_dir, roughness);   
    float G_dir   = GeometrySmith(N, V, L_dir, roughness);      
    vec3 F_dir    = fresnelSchlick(max(dot(H_dir, V), 0.0), F0);
       
    vec3 nom_dir    = NDF_dir * G_dir * F_dir; 
    float denom_dir = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L_dir), 0.0) + 0.0001;
    vec3 specular_dir = nom_dir / denom_dir;
    
    vec3 kS_dir = F_dir;
    vec3 kD_dir = vec3(1.0) - kS_dir;
    kD_dir *= 1.0 - metallic;     

    float NdotL_dir = max(dot(N, L_dir), 0.0);

    float shadow = ShadowCalculation(FragPosLightSpace, N, L_dir);

    Lo += (1.0 - shadow) * (kD_dir * albedo / PI + specular_dir) * radiance_dir * NdotL_dir;

    // Point light

	for(int i = 0; i < activePointLights; ++i) {
        vec3 L = normalize(pointLights[i].position - FragPos);
        vec3 H = normalize(V + L);

        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance); 
        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;

        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        float NdotL = max(dot(N, L), 0.0);

        float shadow = 0.0;
        if (i == 0) {
            shadow = PointShadowCalculation(FragPos, pointLights[i].position);
        }

        Lo += (1.0 - shadow) * (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // SpotLights
    
    for(int i = 0; i < activeSpotLights; ++i) {
        vec3 L = normalize(spotLights[i].position - FragPos);
        vec3 H = normalize(V + L);

        float distance = length(spotLights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance); 
        
        float theta = dot(L, normalize(-spotLights[i].direction)); 
        float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
        float spotIntensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

        vec3 radiance = spotLights[i].color * spotLights[i].intensity * attenuation * spotIntensity;

        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;     

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    
    vec3 F_ambient = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS_ambient = F_ambient;
    vec3 kD_ambient = 1.0 - kS_ambient;
    kD_ambient *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;

    vec3 diffuseAmbient = irradiance * albedo;
    
    vec3 R = reflect(-V, N);
    
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    
    vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    
    vec3 specularAmbient = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
    
    vec3 ambient = (kD_ambient * diffuseAmbient + specularAmbient) * ao;
    
    vec3 color = ambient + Lo;

    vec3 emission = vec3(0.0);
    if (u_HasEmissiveMap) {
        emission = texture(u_EmissiveMap, TexCoord).rgb * u_EmissiveColor * u_EmissiveStrength;
    } else {
        emission = u_EmissiveColor * u_EmissiveStrength;
    }

    color += emission;

    float edgeWidth = 0.05;
    if(noiseVal < u_DissolveAmount + edgeWidth) {
        
        float edgeFactor = (noiseVal - u_DissolveAmount) / edgeWidth;
        
        vec3 edgeColor = vec3(1.0, 0.4, 0.0);
        
        color = mix(edgeColor * 5.0, color, edgeFactor); 
    }

    FragColor = vec4(color, 1.0);

   float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    float threshold = 1.0;
    
    if(brightness > threshold) {
        float excess = brightness - threshold;
        
        vec3 bloomColor = color * (excess / brightness);
        
        BrightColor = vec4(bloomColor, 1.0);
    } else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    OutNormal = vec4(normalize(Normal), 1.0);
    OutlineColor = vec4(u_OutlineColor, 1);
    OutOutlineParams = vec4(u_OutlineSize, u_OutlineDepthThreshold, u_OutlineNormalThreshold, 1.0);
}