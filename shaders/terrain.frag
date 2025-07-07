#version 430 core
#define PI 3.141592653589793238462

in vec2 flatWorldPos;
in vec3 viewPos;
out vec4 FragColor;

layout(std140, binding = 0) uniform terrainParams {
	uniform int octaveCount;
	uniform float initialAmplitude;
	uniform float amplitudeDecay;
	uniform float spreadFactor;
};

uniform float samplingScale;
uniform sampler2D terrainImage;

vec3 getTerrainInfo(vec2 worldPos) {
	vec2 sampleCoord = (worldPos / samplingScale) + vec2(0.5);
	vec3 terrainInfo = texture(terrainImage, sampleCoord).xyz;
	
	if (sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0)
		terrainInfo = vec3(-initialAmplitude, 0, 0);

	terrainInfo.yz /= samplingScale;
	return terrainInfo;
}

float easeInExpo(float x) {
	return x == 0 ? 0 : pow(2.0, 10 * x - 10);
}

void main() {

	vec3 terrainInfo = getTerrainInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));

	//vec3 dirtAlbedo = vec3(0.58, 0.35, 0.22);
	vec3 dirtAlbedo = vec3(0.35, 0.35, 0.35);
	//vec3 grassAlbedo = vec3(0, 0.6, 0);
	vec3 grassAlbedo = vec3(1, 1, 1);

	vec3 lightDir = normalize(vec3(0, 1, 0));
	float diffuse = max(0, dot(lightDir, normal));
	float ambient = 0;

	vec3 albedo = dirtAlbedo + easeInExpo(diffuse) * (grassAlbedo - dirtAlbedo);
	albedo = dirtAlbedo + (diffuse < 0.8 ? 0 : 1) * (grassAlbedo - dirtAlbedo);

	float distFromCamera = length(viewPos);
	float density = 0.07;
	float visibility = 1 / (exp(distFromCamera * density));

	vec3 preFogColor = (diffuse + ambient) * albedo;
	vec3 postFogColor = visibility * preFogColor + (1 - visibility) * vec3(0.5, 0.5, 0.5);
	FragColor = vec4(postFogColor, 1);
}