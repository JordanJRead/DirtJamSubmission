#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

out vec2 flatWorldPos;
out vec3 viewPos;

layout(std140, binding = 2) uniform WaterParams {
	uniform int waveCount;
	uniform float initialAmplitude;
	uniform float amplitudeMult;
	uniform float initialFreq;
	uniform float freqMult;
	uniform float initialSpeed;
	uniform float speedMult;
};

// Per frame
uniform mat4 view;
uniform mat4 proj;
uniform float time;

// Per plane
uniform float planeWorldWidth;
uniform vec3 planePos;

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
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + planePos.x, planePos.y, vPos.y * planeWorldWidth + planePos.z, 1);
	flatWorldPos = worldPos.xz;
	vec3 waterInfo = getWaterHeight(flatWorldPos);
	worldPos.y += waterInfo.x;

	viewPos = (view * worldPos).xyz;
	gl_Position = proj * vec4(viewPos, 1);
}