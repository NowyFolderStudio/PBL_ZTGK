#version 330 core

in vec2 TexCoord;
in vec4 ParticleColor;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 OutlineColor;
layout (location = 3) out vec4 OutNormal;
layout (location = 4) out vec4 OutOutlineParams;

uniform sampler2D u_AlbedoMap;

uniform vec3 u_OutlineColor = vec3(0, 0, 0);
uniform float u_OutlineSize = 0.1;
uniform float u_OutlineDepthThreshold = 0.15;
uniform float u_OutlineNormalThreshold = 1.0;

void main()
{
    vec4 texColor = texture(u_AlbedoMap, TexCoord);
    
    vec4 finalColor = texColor * ParticleColor;
    if(finalColor.a < 0.01)
        discard;

    FragColor = finalColor;

    float brightness = dot(finalColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    
    if(brightness > 1.0)
        BrightColor = vec4(finalColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    OutlineColor = vec4(u_OutlineColor, 1);
    OutOutlineParams = vec4(u_OutlineSize, u_OutlineDepthThreshold, u_OutlineNormalThreshold, 1.0);
}