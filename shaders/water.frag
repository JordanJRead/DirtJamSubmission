#version 430 core
#define PI 3.141592653589793238462

in vec2 flatWorldPos;
in vec3 viewPos;
out vec4 FragColor;

uniform float time;

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
};

layout(std140, binding = 2) uniform WaterParams {
	uniform int waveCount;
	uniform float initialAmplitude;
	uniform float amplitudeMult;
	uniform float initialFreq;
	uniform float freqMult;
	uniform float initialSpeed;
	uniform float speedMult;
};

uint rand(uint n) {
	uint state = n * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	word = (word >> 22u) ^ word;
	return word;
}

float randToFloat(uint n) {
	return float(n) / 4294967296.0;
}

vec2 randUnitVector(float randNum) {
	float theta = 2 * PI * randNum;
	return normalize(vec2(cos(theta), sin(theta)));
}

vec3 getWaterHeight(vec2 pos) {
	vec3 waterInfo = vec3(0, 0, 0);

	float amplitude = initialAmplitude;
	float freq = initialFreq;
	float speed = initialSpeed;

	for (int i = 0; i < waveCount; ++i) {
		float randNum = randToFloat(rand(i));
		vec2 waterDir = randUnitVector(randNum);
		waterInfo.x += amplitude * sin(dot(waterDir, pos) * freq + time * speed);
		waterInfo.yz += amplitude * cos(dot(waterDir, pos) * freq + time * speed) * freq * waterDir;

		amplitude *= amplitudeMult;
		freq *= freqMult;
		speed *= speedMult;
	}
	return waterInfo / waveCount;
}

void main() {
	// Lighting
	vec3 waterInfo = getWaterHeight(flatWorldPos);
	vec3 normal = normalize(vec3(-waterInfo.y, 1, -waterInfo.z));
	vec3 lightDir = normalize(vec3(0, 1, 0));
	float diffuse = max(0, dot(lightDir, normal));
	float ambient = 0;

	vec3 albedo = vec3(0, 0.2, 0.7);

	float distFromCamera = length(viewPos);
	float fogStart = maxViewDistance - fogEncroach;
	float fogStrength;

	if (distFromCamera < fogStart)
		fogStrength = 0;
	else if (distFromCamera > maxViewDistance)
		fogStrength = 1;
	else
		fogStrength = (distFromCamera - fogStart) / fogEncroach;

	vec3 preFogColor = (diffuse + ambient) * albedo;
	vec3 postFogColor = (1 - fogStrength) * preFogColor + fogStrength * vec3(0.5, 0.5, 0.5);
	FragColor = vec4(postFogColor, 1);
}