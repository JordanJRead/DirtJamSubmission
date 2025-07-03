#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

uniform mat4 view;
uniform mat4 proj;
uniform float scale;
uniform float latticeWidth;

uniform int octaveCount;
uniform float initialAmplitude;
uniform float amplitudeDecay;
uniform float spreadFactor;

out flat float n;

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

float perlin(vec2 pos) {
	int x0 = getClosestInt(floor(pos.x));
	int x1 = getClosestInt(ceil(pos.x));
	int y0 = getClosestInt(floor(pos.y));
	int y1 = getClosestInt(ceil(pos.y));

	vec2 p00 = vec2(x0, y0);
	
	vec2 relPoint =  pos - p00; // true

	float r00 = randToFloat(rand(labelPoint(x0, y0))); // true
	float r01 = randToFloat(rand(labelPoint(x0, y1)));
	float r10 = randToFloat(rand(labelPoint(x1, y0)));
	float r11 = randToFloat(rand(labelPoint(x1, y1)));

	vec2 g00 = randUnitVector(r00);
	vec2 g01 = randUnitVector(r01);
	vec2 g10 = randUnitVector(r10);
	vec2 g11 = randUnitVector(r11);

	vec2 v00 = relPoint;
	vec2 v11 = relPoint - vec2(1, 1);
	vec2 v01 = vec2(v00.x, v11.y);
	vec2 v10 = vec2(v11.x, v00.y);
	
	float d00 = dot(v00, g00);
	float d01 = dot(v01, g01);
	float d10 = dot(v10, g10);
	float d11 = dot(v11, g11);

	// From https://iquilezles.org/articles/gradientnoise/ and Acerola's github
	vec2 u = quinticInterpolation(relPoint);
	float noise = d00 + u.x * (d10 - d00) + u.y * (d01 - d00) + u.x * u.y * (d00 - d10 - d01 + d11);

	return noise * 0.5f + 0.5;
}

void main() {
	vec4 worldPos = vec4(vPos.x * scale, 0, vPos.y * scale, 1);
	vec4 latticePos = vec4(worldPos.xyz / latticeWidth, 1);

	/*
	
uniform int octaveCount;
uniform float initialAmplitude;
uniform float amplitudeDecay;
uniform float spreadFactor;

	*/

	float amplitude = initialAmplitude;
	float spread = 1;
	for (int i = 0; i < octaveCount; ++i) {
		vec2 samplePos = latticePos.xz * spread;
		worldPos += vec4(0, amplitude * perlin(samplePos), 0, 0);
		amplitude *= amplitudeDecay;
		spread *= spreadFactor;
	}
	gl_Position = proj * view * worldPos;

	n = perlin(latticePos.xz);
}