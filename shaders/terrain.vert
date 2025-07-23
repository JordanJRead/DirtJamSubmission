#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

out vec2 flatWorldPos;
out vec3 viewPos;

// Per app
uniform int imageCount;
uniform sampler2D images[3];

// Per whenever they get changed
uniform float imageScales[3];
uniform vec2 imagePositions[3];

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
uniform mat4 view;
uniform mat4 proj;

// Per plane
uniform float planeWorldWidth;
uniform vec3 planePos;
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

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + planePos.x, planePos.y, vPos.y * planeWorldWidth + planePos.z, 1);
	flatWorldPos = worldPos.xz;
	vec3 terrainInfo = getTerrainInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	worldPos.y += terrainInfo.x;

	shellIndex = gl_InstanceID - 1;
	if (shellIndex >= 0) {
		float shellProgress = float(shellIndex + 1) / shellCount;
		worldPos.xyz += normal * shellProgress * shellMaxHeight;
	}

	viewPos = (view * worldPos).xyz;
	gl_Position = proj * vec4(viewPos, 1);
}