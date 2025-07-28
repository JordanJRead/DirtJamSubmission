#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

out vec2 flatWorldPos;
out vec3 viewPos;
out flat int shellIndex;

// Buffers
layout(binding = 0, std430) buffer ssbo0 {
	int[] ssboChunkXZ; // Size = 2 * number of chunks
};

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

layout(binding = 1, std430) buffer sssbo1 { // Changes twice per frame
	int[] visibleChunkIndices; // Size = number of visible chunks
};

/// Uniforms
// Per app
uniform int imageCount;
uniform sampler2D images[3];

// Per whenever they get changed
uniform int chunkWidth;
uniform float lowQualityPlaneStepSize;
uniform float imageScales[3];
uniform vec2 imagePositions[3];

// Per frame
uniform vec3 cameraPos;
uniform mat4 view;
uniform mat4 proj;
uniform int instanceCount;


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
	float stepSize = lowQualityPlaneStepSize * chunkWidth;
	vec3 stepSizesAway = pos / stepSize;
	stepSizesAway = vec3(int(stepSizesAway.x), int(stepSizesAway.y), int(stepSizesAway.z));
	return stepSizesAway * stepSize;
}

void main() {
	
	int chunkCount = instanceCount / (shellCount + 1);
	int chunkIndex = visibleChunkIndices[gl_InstanceID % chunkCount];
	int posIndex = chunkIndex * 2;
	int x = ssboChunkXZ[posIndex];
	int z = ssboChunkXZ[posIndex + 1];

	vec3 planePos = getClosestWorldVertexPos(cameraPos - vec3(x * chunkWidth, 0, z * chunkWidth));
	planePos = vec3(planePos.x, 0, planePos.z);
	vec4 worldPos = vec4(vPos.x * chunkWidth + planePos.x, planePos.y, vPos.y * chunkWidth + planePos.z, 1);
	flatWorldPos = worldPos.xz;
	vec3 terrainInfo = getTerrainInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	worldPos.y += terrainInfo.x;

	shellIndex = gl_InstanceID / chunkCount - 1;
	if (shellIndex >= 0) {
		float shellProgress = float(shellIndex + 1) / shellCount;
		worldPos.xyz += normal * shellProgress * shellMaxHeight;
	}

	viewPos = (view * worldPos).xyz;
	gl_Position = proj * vec4(viewPos, 1);
}