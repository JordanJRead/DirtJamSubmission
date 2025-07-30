#version 430 core

in vec3 outVPos;

out vec4 FragColor;

uniform samplerCube skybox;
uniform vec3 dirToLight;

layout(std140, binding = 2) uniform WaterParams {
	uniform int waveCount;
	uniform float initialAmplitude;
	uniform float amplitudeMult;
	uniform float initialFreq;
	uniform float freqMult;
	uniform float initialSpeed;
	uniform float speedMult;
	uniform float specExp;
	uniform vec3 sunColour;
};

void main() {
	float d = dot(normalize(outVPos), dirToLight);
	d = clamp(d, 0.0, 1.0);
	vec3 sunAdd = pow(d, 200.0) * sunColour;
	FragColor = texture(skybox, outVPos) + vec4(sunAdd, 1);
}