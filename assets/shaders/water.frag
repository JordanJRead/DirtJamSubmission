#version 430 core
#define PI 3.141592653589793238462

in vec3 worldPos3;
in vec3 viewPos;
out vec4 FragColor;

uniform float time;
uniform samplerCube skybox;
uniform vec3 cameraPos;
uniform vec3 dirToLight;

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
	uniform float specExp;
	uniform vec3 sunColor;
};

layout(std140, binding = 3) uniform Colours {
	uniform vec3 dirtColor;
	uniform vec3 mountainColor;
	uniform vec3 grassColor;
	uniform vec3 snowColor;
	uniform vec3 waterColor;
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

	float amplitudeSum = 0;

	for (int i = 0; i < waveCount; ++i) {
		amplitudeSum += amplitude;
		float randNum = randToFloat(rand(i));
		vec2 waterDir = randUnitVector(randNum);
		//waterInfo.x += amplitude * sin(dot(waterDir, pos) * freq + time * speed);
		waterInfo.x += amplitude * (exp(sin(dot(waterDir, pos) * freq + time * speed)) - 1.4);
		waterInfo.yz += amplitude * exp(sin(dot(waterDir, pos) * freq + time * speed)) * cos(dot(waterDir, pos) * freq + time * speed) * freq * waterDir;

		amplitude *= amplitudeMult;
		freq *= freqMult;
		speed *= speedMult;
	}
	return waterInfo / amplitudeSum * initialAmplitude;
}

void main() {
	// Water
	vec2 flatWorldPos = worldPos3.xz;
	vec3 waterInfo = getWaterHeight(flatWorldPos);

	// Lighting
	vec3 normal = normalize(vec3(-waterInfo.y, 1, -waterInfo.z));
	float diffuse = max(0, dot(dirToLight, normal));

	vec3 viewDir = normalize(cameraPos - worldPos3);
	vec3 halfWay = normalize(viewDir + dirToLight);
	float spec = pow(max(dot(normal, halfWay), 0), specExp);

	float ambient = 0.2;


	vec3 albedo = waterColor;

	float distFromCamera = length(viewPos);
	float fogStart = maxViewDistance - fogEncroach;
	float fogStrength;

	if (distFromCamera < fogStart)
		fogStrength = 0;
	else if (distFromCamera > maxViewDistance)
		fogStrength = 1;
	else
		fogStrength = (distFromCamera - fogStart) / fogEncroach;
		
	vec3 skyboxSample = worldPos3 - cameraPos;
	vec3 litAlbedo = (diffuse + ambient) * albedo;
	
	float fresnel = pow(1 - dot(viewDir, normal), 3.0);
	vec3 reflectDir = normalize(reflect(-viewDir, normal));
	vec3 reflectColor = texture(skybox, reflectDir).xyz;

	fresnel = clamp(fresnel, 0.0, 1.0);
	litAlbedo = fresnel * reflectColor + (1 - fresnel) * litAlbedo + spec * sunColor;

	vec3 finalColor = (1 - fogStrength) * litAlbedo + fogStrength * texture(skybox, skyboxSample).xyz;

	FragColor = vec4(finalColor, 1);
}