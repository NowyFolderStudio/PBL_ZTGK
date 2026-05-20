#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

in mat3 TBN;

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

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

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

void main() {
	vec3 albedo = u_HasAlbedoMap ? pow(texture(u_AlbedoMap, TexCoord).rgb, vec3(2.2)) : u_AlbedoColor;

	vec3 N = normalize(Normal);

    if (u_HasNormalMap) {
        vec3 normalMap = texture(u_NormalMap, TexCoord).rgb;
        normalMap = normalMap * 2.0 - 1.0;
        
        N = normalize(TBN * normalMap);
    }

	float metallic = u_HasMetallicMap ? texture(u_MetallicMap, TexCoord).r : u_Metallic;
    float roughness = u_HasRoughnessMap ? texture(u_RoughnessMap, TexCoord).r : u_Roughness;
    float ao = u_HasAOMap ? texture(u_AOMap, TexCoord).r : 1.0;

	vec3 V = normalize(viewPos - FragPos); // Wektor od fragmentu do camery

	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);

    // Œwiatlo kierunkowe
 
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
    Lo += (kD_dir * albedo / PI + specular_dir) * radiance_dir * NdotL_dir;

    // Œwiat³o punktowe

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

        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
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

    FragColor = vec4(color, 1.0);
}