#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

out vec2 flatWorldPos;
out vec3 viewPos;

// Per app
uniform int imageCount;
uniform sampler2D images[3];

// Per whenever they get changed
uniform float planeWorldWidth;
uniform float lowQualityPlaneStepSize;
uniform float imageScales[3];
uniform vec2 imagePositions[3];

layout(binding = 0, std430) buffer ssbo0 {
	int[] ssboHighQualityXZ; // Size = 2 * number of chunks
}; //TODO make mChunkWidth update automatically in the shader

layout(binding = 1, std430) buffer ssbo1 {
	int[] ssboLowQualityXZ; // Size = 2 * number of chunks
}; //TODO make mChunkWidth update automatically in the shader

layout(std140, binding = 1) uniform ArtisticParams {
	uniform float terrainScale;
	uniform float fogStrength;
	uniform float colorDotCutoff;
	uniform int shellCount;
	uniform float shellMaxHeight;
	uniform float shellDetail;
	uniform float shellMaxCutoff;
	uniform float shellBaseCutoff;
};

// Per frame
uniform vec3 cameraPos;
uniform mat4 view;
uniform mat4 proj;
layout(binding = 2, std430) buffer sssbo2 {
	int[] visibleChunkIndices; // Size = number of visible chunks
};

uniform bool isHighQuality;

// Per plane
out flat int shellIndex;

vec3 getTerrainInfo(vec2 worldPos) {
	for (int i = 0; i < 3; ++i) {
		vec2 sampleCoord = ((worldPos / terrainScale - imagePositions[i]) / imageScales[i]) + vec2(0.5);
		
		if (!(sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0)) {
			vec3 terrainInfo = texture(images[i], sampleCoord).rgb;
			terrainInfo.yz /= imageScales[i] * terrainScale;
			return terrainInfo;
		}
	}
	return vec3(0, 0, 0);
}

vec3 getClosestWorldVertexPos(vec3 pos) {
	float stepSize = lowQualityPlaneStepSize * planeWorldWidth;
	vec3 stepSizesAway = pos / stepSize;
	stepSizesAway = vec3(int(stepSizesAway.x), int(stepSizesAway.y), int(stepSizesAway.z));
	return stepSizesAway * stepSize;
}

void main() {
	int chunkIndex = visibleChunkIndices[gl_InstanceID / (shellIndex + 1)];
	int posIndex = chunkIndex * 2;
	int x = isHighQuality ? ssboHighQualityXZ[posIndex] : ssboLowQualityXZ[posIndex];
	int z = isHighQuality ? ssboHighQualityXZ[posIndex + 1] : ssboLowQualityXZ[posIndex + 1];

	vec3 planePos = getClosestWorldVertexPos(cameraPos - vec3(x * planeWorldWidth, 0, z * planeWorldWidth));
	planePos = vec3(planePos.x, 0, planePos.z);
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + planePos.x, planePos.y, vPos.y * planeWorldWidth + planePos.z, 1);
	flatWorldPos = worldPos.xz;
	vec3 terrainInfo = getTerrainInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	worldPos.y += terrainInfo.x;

	shellIndex = gl_InstanceID % (shellCount + 1) - 1;
	if (shellIndex >= 0) {
		float shellProgress = float(shellIndex + 1) / shellCount;
		worldPos.xyz += normal * shellProgress * shellMaxHeight;
	}

	viewPos = (view * worldPos).xyz;
	gl_Position = proj * vec4(viewPos, 1);
}