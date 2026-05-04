#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float u_MusicTime;
uniform float u_ScaleStrengthY;
uniform float u_ScaleStrengthXZ;
uniform float u_BendStrength;
uniform float u_TwistStrength;

void main() {
	float wave = sin(u_MusicTime * 3.14159);

	vec3 localPos = aPos;

	float scaleY = 1.0 + (wave * u_ScaleStrengthY);
	localPos.y *= scaleY;

	float scaleXZ = 1.0 + (wave * u_ScaleStrengthXZ);
	localPos.x *= scaleXZ;
	localPos.z *= scaleXZ;

	// twist
	float twistAngle = wave * u_TwistStrength * localPos.y;

	float s = sin(twistAngle);
	float c = cos(twistAngle);

	vec3 twistedPos = localPos;
	twistedPos.x = localPos.x * c - localPos.z * s;
	twistedPos.z = localPos.x * s + localPos.z * c;

	// bend
	float bendOffset = pow(twistedPos.y, 2.0) * wave * u_BendStrength;

	vec3 finalLocalPos = twistedPos + vec3(bendOffset, 0.0, 0.0);

	FragPos = vec3(model * vec4(finalLocalPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoord = aTexCoord;

	gl_Position = projection * view * vec4(FragPos, 1.0);
}