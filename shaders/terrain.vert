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
out vec3 viewPos;

vec3 getTerrainInfo(vec2 worldPos) {
	vec2 sampleCoord = (worldPos / samplingScale) + vec2(0.5);
	vec3 terrainInfo = texture(terrainImage, sampleCoord).xyz;

	if (sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0)
		terrainInfo = vec3(-initialAmplitude, 0, 0);

	terrainInfo.yz /= samplingScale;
	return terrainInfo;
}

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth, 0, vPos.y * planeWorldWidth, 1);
	flatWorldPos = worldPos.xz;
	vec3 terrainInfo = getTerrainInfo(flatWorldPos);
	worldPos.y += terrainInfo.x;

	viewPos = (view * worldPos).xyz;
	gl_Position = proj * vec4(viewPos, 1);
}