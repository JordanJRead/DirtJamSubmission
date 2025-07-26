#version 430 core
#define PI 3.141592653589793238462

in vec2 flatWorldPos;
in vec3 viewPos;
out vec4 FragColor;

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

void main() {

	// Lighting
	vec3 normal = vec3(0, 1, 0);
	vec3 lightDir = normalize(vec3(0, 1, 0));
	float diffuse = max(0, dot(lightDir, normal));
	float ambient = 0;

	vec3 albedo = vec3(0, 0.2, 0.7);

	float distFromCamera = length(viewPos);
	float visibility = exp(-distFromCamera * fogStrength);

	vec3 preFogColor = (diffuse + ambient) * albedo;
	vec3 postFogColor = visibility * preFogColor + (1 - visibility) * vec3(0.5, 0.5, 0.5);
	FragColor = vec4(postFogColor, 1);
}