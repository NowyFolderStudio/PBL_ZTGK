#version 330 core

in vec2 TexCoord;
in vec4 ParticleColor;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D u_AlbedoMap;

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
}