#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

uniform mat4 view;
uniform mat4 proj;
uniform float planeWorldWidth;
uniform float samplingScale;
uniform float samplingScaleBad;

layout(std140, binding = 0) uniform terrainParams {
	uniform int octaveCount;
	uniform float initialAmplitude;
	uniform float amplitudeDecay;
	uniform float spreadFactor;
};

uniform sampler2D terrainImage;
uniform sampler2D terrainImageBad;
uniform int shellIndex;
uniform float extrudePerShell;

out vec2 flatWorldPos;
out vec3 viewPos;

vec3 getTerrainInfo(vec2 worldPos) {
	vec2 sampleCoord = (worldPos / samplingScale) + vec2(0.5);
	vec3 terrainInfo = texture(terrainImage, sampleCoord).xyz;
	bool badTexture = false;

	if (sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0) {
		badTexture = true;
		sampleCoord = (worldPos / samplingScaleBad) + vec2(0.5);
		terrainInfo = texture(terrainImageBad, sampleCoord).xyz;
		if (sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0) {
			terrainInfo = vec3(-initialAmplitude, 0, 0);
		}
	}

	terrainInfo.yz /= badTexture ? samplingScaleBad : samplingScale;
	return terrainInfo;
}

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth, 0, vPos.y * planeWorldWidth, 1);
	flatWorldPos = worldPos.xz;
	vec3 terrainInfo = getTerrainInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	worldPos.y += terrainInfo.x;
	worldPos.xyz += normal * shellIndex * extrudePerShell;

	viewPos = (view * worldPos).xyz;
	gl_Position = proj * vec4(viewPos, 1);
}