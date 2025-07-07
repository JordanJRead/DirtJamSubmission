#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

uniform mat4 view;
uniform mat4 proj;
uniform float planeWorldWidth;
uniform float samplingScale;

layout(std140, binding = 0) uniform terrainParams {
	uniform int octaveCount;
	uniform float initialAmplitude;
	uniform float amplitudeDecay;
	uniform float spreadFactor;
};

uniform sampler2D terrainImage;

out vec2 flatWorldPos;

vec3 getTerrainInfo(vec2 worldPos) {
	vec3 terrainInfo = texture(terrainImage, (worldPos / samplingScale) + vec2(0.5)).xyz;
	terrainInfo.yz /= samplingScale;
	return terrainInfo;
}

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth, 0, vPos.y * planeWorldWidth, 1);
	flatWorldPos = worldPos.xz;
	vec3 terrainInfo = getTerrainInfo(flatWorldPos);
	worldPos.y += terrainInfo.x;

	gl_Position = proj * view * worldPos;
}