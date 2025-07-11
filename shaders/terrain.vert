#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

uniform mat4 view;
uniform mat4 proj;
uniform float planeWorldWidth;

uniform int imageCount;
uniform float imageScales[];
uniform sampler2D images[];
uniform vec3 imagePositions[];

uniform int shellIndex;
uniform float extrudePerShell;

out vec2 flatWorldPos;
out vec3 viewPos;

vec3 getTerrainInfo(vec2 worldPos) {
	for (int i = 0; i < imageCount; ++i) {
		vec2 sampleCoord = ((worldPos - imagePositions[i]) / imageScales[i]) + vec2(0.5);
		
		if (!(sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0)) {
			vec3 terrainInfo = texture(images[i], sampleCoord).rgb;
			terrainInfo.yz /= imageScales[i];
			return terrainInfo;
		}
	}
	return vec3(0, 0, 0);
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