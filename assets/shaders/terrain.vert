#version 430 core
#define PI 3.141592653589793238462
#define IMAGECOUNT 4

layout(location = 0) in vec2 vPos;

out vec3 viewPos;
out vec3 groundWorldPos;
out vec3 shellWorldPos;

// Per app
uniform int imageCount;
uniform sampler2D images[IMAGECOUNT];

// Per whenever they get changed
uniform float imageScales[IMAGECOUNT];
uniform vec2 imagePositions[IMAGECOUNT];

layout(std140, binding = 1) uniform ArtisticParams {
	uniform float terrainScale;
	uniform float maxViewDistance;
	uniform float fogEncroach;
	uniform float colorDotCutoff;
	uniform int shellCount;
	uniform float shellMaxHeight;
	uniform float shellDetail;
	uniform float shellMaxCutoff;
	uniform float shellBaseCutoff;
	uniform float snowHeight;
	uniform float seafoamStrength;
};

// Per frame
uniform mat4 view;
uniform mat4 proj;

// Per plane
uniform float planeWorldWidth;
uniform vec3 planePos;
out flat int shellIndex;

vec4 getTerrainInfo(vec2 worldPos) {
	for (int i = 0; i < IMAGECOUNT; ++i) {
		vec2 sampleCoord = ((worldPos / terrainScale - imagePositions[i]) / imageScales[i]) + vec2(0.5);
		
		if (!(sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0)) {
			vec4 terrainInfo = texture(images[i], sampleCoord);
			terrainInfo.yz /= imageScales[i] * terrainScale;
			return terrainInfo;
		}
	}
	return vec4(0, 0, 0, 0);
}

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + planePos.x, planePos.y, vPos.y * planeWorldWidth + planePos.z, 1);
	vec2 flatWorldPos = worldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	worldPos.y += terrainInfo.x;
	
	groundWorldPos = worldPos.xyz;
	shellIndex = gl_InstanceID - 1;
	if (shellIndex >= 0) {
		float shellProgress = float(shellIndex + 1) / shellCount;
		worldPos.xyz += normal * shellProgress * shellMaxHeight;
	}
	shellWorldPos = worldPos.xyz;
	
	viewPos = (view * worldPos).xyz;
	gl_Position = proj * vec4(viewPos, 1);
}