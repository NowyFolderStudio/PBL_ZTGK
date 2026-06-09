#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlurTexture;
uniform float exposure;

// Outline parameters
uniform sampler2D outlineColorTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D outlineParamsTexture;

// ====== OUTLINE ======
float GetLinearDepth(vec2 uv) {
    // This 2 values has to adjusted to camera settings
	float zNear = 0.1;
    float zFar = 1000;
    float depth = texture(depthTexture, uv).r;
    return (2.0 * zNear * zFar) / (zFar + zNear - (depth * 2.0 - 1.0) * (zFar - zNear));
}

bool IsEdge(vec2 texCoord, float thickness, float depthThreshold, float normalThreshold) { 
    vec2 texelSize = vec2(1.0 / 1920.0, 1.0 / 1080.0);

    vec2 samplePoints[9] = vec2[](
        vec2(-1,  1), vec2(0,  1), vec2(1,  1),
        vec2(-1,  0), vec2(0,  0), vec2(1,  0),
        vec2(-1, -1), vec2(0, -1), vec2(1, -1)
    );

    float sobelX[9] = float[](1, 0, -1, 2, 0, -2, 1, 0, -1);
    float sobelY[9] = float[](1, 2, 1, 0, 0, 0, -1, -2, -1);

    vec2 sobelDepth = vec2(0.0);
    vec3 sobelNormalX = vec3(0.0);
    vec3 sobelNormalY = vec3(0.0);

    float centerDepth = GetLinearDepth(texCoord);
    float minDepth = centerDepth;

    for(int i = 0; i < 9; i++) {
        vec2 uvOffset = samplePoints[i] * texelSize * thickness;
        
        float depth = GetLinearDepth(texCoord + uvOffset);
        sobelDepth += depth * vec2(sobelX[i], sobelY[i]);

        if (depth < minDepth) {
            minDepth = depth;
        }

        vec3 normal = texture(normalTexture, texCoord + uvOffset).rgb;
        sobelNormalX += normal * sobelX[i];
        sobelNormalY += normal * sobelY[i];
    }

    float depthEdge = length(sobelDepth);
    float normalEdge = length(sobelNormalX) + length(sobelNormalY);

    if (centerDepth > minDepth + 0.2) {
        return false;
    }

    return (depthEdge > depthThreshold) || (normalEdge > normalThreshold);
}

void main() {
	const float gamma = 2.2;
	vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
	vec3 bloomColor = texture(bloomBlurTexture, TexCoords).rgb;

	hdrColor += bloomColor;

	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	mapped = pow(mapped, vec3(1.0 / gamma));

	vec4 finalColor = vec4(mapped, 1.0);

    // ====== OUTLINE ======
	float rawDepth = texture(depthTexture, TexCoords).r;

    if (rawDepth < 0.99999) {

        vec3 params = texture(outlineParamsTexture, TexCoords).rgb;
        float currentSize   = params.r;
        float currentDepthT = params.g;
        float currentNormT  = params.b;

        if (IsEdge(TexCoords, currentSize, currentDepthT, currentNormT)) {
            vec4 edgeColorSample = texture(outlineColorTexture, TexCoords);
            if (edgeColorSample.a > 0.01) {
                finalColor = vec4(edgeColorSample.rgb, 1.0);
            } else {
                finalColor = vec4(0.0, 0.0, 0.0, 1.0);
            }
        }
        
    }
	FragColor = finalColor;
}