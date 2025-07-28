#version 430 core
#define PI 3.141592653589793238462

in vec2 latticePos;
out vec4 OutTerrainData;

layout(std140, binding = 0) uniform terrainParams {
	uniform int octaveCount;
	uniform float initialAmplitude;
	uniform float amplitudeDecay;
	uniform float spreadFactor;
	uniform float roughness;
};

uniform float scale;

uint rand(uint n) {
	uint state = n * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	word = (word >> 22u) ^ word;
	return word;
}

float randToFloat(uint n) {
	return float(n) / 4294967296.0;
}

uint labelPoint(int x, int y) {
	if (x == 0 && y == 0)
		return 0;

	int n = max(abs(x), abs(y));
	int width = 2 * n + 1;
	int startingIndex = (width - 2) * (width - 2);

	if (n == y) { // top row
		return startingIndex + x + n;
	}
	if (n == -y) { // bottom row
		return startingIndex + width + x + n;
	}
	if (n == x) { // right col
		return startingIndex + width * 2 + y - 1 + n;
	}
	if (n == -x) { // right col
		return startingIndex + width * 2  + width - 2 + y - 1 + n;
	}
	return 0;
}

vec2 randUnitVector(float randNum) {
	float theta = 2 * PI * randNum;
	return normalize(vec2(cos(theta), sin(theta)));
}

int getClosestInt(float x) {
	return int(round(x) + 0.1 * (x < 0 ? -1 : 1));
}

vec2 quinticInterpolation(vec2 t) {
	return t * t * t * (t * (t * vec2(6) - vec2(15)) + vec2(10));
}

vec2 quinticDerivative(vec2 t) {
	return vec2(30) * t * t * (t * (t - vec2(2)) + vec2(1));
}

vec3 perlin(vec2 pos, int reroll = 0) {
	int x0 = getClosestInt(floor(pos.x));
	int x1 = getClosestInt(ceil(pos.x));
	int y0 = getClosestInt(floor(pos.y));
	int y1 = getClosestInt(ceil(pos.y));

	vec2 p00 = vec2(x0, y0);
	
	vec2 relPoint =  pos - p00;

	uint rui00 = rand(labelPoint(x0, y0));
	uint rui10 = rand(labelPoint(x1, y0));
	uint rui01 = rand(labelPoint(x0, y1));
	uint rui11 = rand(labelPoint(x1, y1));

	for (int i = 0; i < reroll; ++i) {
		rui00 = rand(rui00);
		rui10 = rand(rui10);
		rui01 = rand(rui01);
		rui11 = rand(rui11);
	}

	float r00 = randToFloat(rui00);
	float r10 = randToFloat(rui10);
	float r01 = randToFloat(rui01);
	float r11 = randToFloat(rui11);

	vec2 g00 = randUnitVector(r00);
	vec2 g10 = randUnitVector(r10);
	vec2 g01 = randUnitVector(r01);
	vec2 g11 = randUnitVector(r11);

	vec2 v00 = relPoint;
	vec2 v11 = relPoint - vec2(1, 1);
	vec2 v10 = relPoint - vec2(1, 0);
	vec2 v01 = relPoint - vec2(0, 1);
	
	float d00 = dot(v00, g00);
	float d10 = dot(v10, g10);
	float d01 = dot(v01, g01);
	float d11 = dot(v11, g11);

	// From https://iquilezles.org/articles/gradientnoise/ and Acerola's github
	vec2 u = quinticInterpolation(relPoint);
	vec2 du = quinticDerivative(relPoint);
	float noise = d00 + u.x * (d10 - d00) + u.y * (d01 - d00) + u.x * u.y * (d00 - d10 - d01 + d11);
	noise = noise / 1.414 + 0.5;
	vec2 tangents = g00 + u.x * (g10 - g00) + u.y * (g01 - g00) + u.x * u.y * (g00 - g10 - g01 + g11) + du * (u.yx * (d00 - d10 - d01 + d11) + vec2(d10, d01) - d00);
	tangents /= 1.414;

	return vec3(noise, tangents.x, tangents.y);
}

// Lowers the amplitude for terrain points below y=0 to create flat ground
float amp(float x, float rough) {
	if (x > 0) {
		return 1;
	}
	return rough / (x * x + rough);
}

float damp(float x, float rough) {
	if (x > 0)
		return 0;
	return -2 * rough * x / (x * x + rough) / (x * x + rough);
}

vec3 getTerrainInfo(vec2 pos) {
	vec3 mountain = perlin(pos * 0.2);
	mountain.yz *= scale;
	mountain.yz *= 0.2;
	
	mountain.yz = 2 * mountain.x * mountain.yz;
	mountain.x *= mountain.x;
	mountain.yz = 2 * mountain.x * mountain.yz;
	mountain.x *= mountain.x;
	mountain.x = mountain.x * 0.95 + 0.05;
	mountain.yz = 0.9 * mountain.yz;

	vec3 offset = perlin(pos * 0.08, 1);

	offset.x = offset.x < 0.5 ? (16*offset.x*offset.x*offset.x*offset.x*offset.x) : 1 - pow(-2 * offset.x + 2, 5.0) / 2.0;
	offset.yz *= offset.x < 0.5 ? 80 * offset.x * offset.x*offset.x*offset.x : 80 * (offset.x-1)*(offset.x-1)*(offset.x-1)*(offset.x-1);
	
	offset.yz *= scale;
	offset.yz *= 0.08;
	offset *= 20;

	vec3 terrainInfo = vec3(0, 0, 0);

	float amplitude = initialAmplitude;
	float spread = 1;

	for (int i = 0; i < octaveCount; ++i) {
		vec2 samplePos = pos * spread;
		vec3 perlinData = perlin(samplePos);

		terrainInfo.x += amplitude * perlinData.x;
		terrainInfo.yz += amplitude * perlinData.yz * spread;

		amplitude *= amplitudeDecay;
		spread *= spreadFactor;
	}
	terrainInfo.yz *= scale;
	
	float amp = amp(terrainInfo.x, roughness);
	amp = 1;
	float damp = damp(terrainInfo.x, roughness);
	damp = 0;

	vec3 finalOutput = vec3(0);
	finalOutput.x = terrainInfo.x * mountain.x;
	finalOutput.yz = terrainInfo.x * mountain.yz + mountain.x * terrainInfo.yz;

	finalOutput.x += offset.x;
	finalOutput.yz += offset.yz;
	return finalOutput;
}

void main() {
	OutTerrainData = vec4(getTerrainInfo(latticePos), 1);
}