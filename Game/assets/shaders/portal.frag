#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 OutlineColor;
layout (location = 3) out vec4 OutNormal;
layout (location = 4) out vec4 OutOutlineParams;

in vec2 TexCoord;
in vec3 Normal;

uniform float u_Time;
uniform vec3 u_PortalColor;
uniform float u_BloomIntensity;

#define PI 3.14159265359

vec2 hash22(vec2 p) {
    vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.xx + p3.yz) * p3.zy);
}

float hash11(float p) {
    return length(hash22(vec2(p)));
}

float hash21(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

mat2 rmatrix(float a) {    
    float c = cos(a);
    float s = sin(a);
    return mat2(c, -s, s, c);
}

float S(float x) {
    return (3.0 * x * x - 2.0 * x * x * x);
}

float noise(vec2 pos) {
    float a = hash21(vec2(floor(pos.x), floor(pos.y)));
    float b = hash21(vec2(ceil(pos.x), floor(pos.y)));
    float c = hash21(vec2(floor(pos.x), ceil(pos.y)));
    float d = hash21(vec2(ceil(pos.x), ceil(pos.y)));

    float s1 = S(pos.x - floor(pos.x));
    float s2 = S(pos.y - floor(pos.y));

    return a + (b - a) * s1 + (c - a) * s2 + (a - b - c + d) * s1 * s2;
}

float onoise(in vec2 pos) {
    float n = 3.0;
    float delta = 3.1415 / 6.0;
    float sum = 0.0;
    float power = 0.5;

    for (float i = 0.0; i < n; i++) {
        sum += noise(rmatrix(delta * i) * pos) * power;
        power *= 0.40;
        pos *= 1.9;
    }
    return sum;
}

vec3 portalTexture(vec2 uv) {
    vec2 uv2 = vec2(length(uv), (atan(uv.y, uv.x) + PI) / (2.0 * PI));
    
    uv2.y = fract(uv2.y + uv2.x * 0.3 - u_Time * 0.01);
    uv2.x = uv2.x * 1.0 + u_Time * 0.3;
    
    vec3 baseCol = length(u_PortalColor) > 0.01 ? u_PortalColor : vec3(0.9f, 0.15f, 0.0f);
    
    vec3 colors[4] = vec3[4]( 
        baseCol * 0.25,
        baseCol * 0.65,
        baseCol,
        mix(baseCol, vec3(1.0), 0.7)
    );
    
    vec2 k = vec2(10.0);
    float br1 = onoise(uv2 * k);
    float br2 = onoise(vec2(uv2.x, uv2.y - 1.0) * k);
    float br = mix(br1, br2, uv2.y);
    
    br = min(0.99, pow(abs(br * 1.5), 2.5));
    
    int bri = int(br / 0.25);
    return colors[bri];
}

void main() {
    vec2 uv = TexCoord * 2.0 - 1.0;
    
    uv.y *= (7.0 / 6.0);

    if (length(uv) > 0.95) {
        discard;
    }

    vec3 finalColor = portalTexture(uv);

    FragColor = vec4(finalColor, 1.0);
    
    float bloomLimit = u_BloomIntensity > 0.01 ? u_BloomIntensity : 0.4;
    BrightColor = vec4(finalColor * bloomLimit, 1.0); 
    
    OutlineColor = vec4(0.0, 0.0, 0.0, 1.0);
    OutNormal = vec4(normalize(Normal), 1.0);
    OutOutlineParams = vec4(0.08, 1.1, 0.30, 1.0);
}