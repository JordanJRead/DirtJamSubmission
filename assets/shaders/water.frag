#version 430 core
#define PI 3.141592653589793238462
#define IMAGECOUNT 4

in vec3 worldPos3;
in vec3 viewPos;
out vec4 FragColour;

uniform float time;
uniform samplerCube skybox;
uniform vec3 cameraPos;
uniform vec3 dirToLight;

uniform sampler2D images[IMAGECOUNT];
uniform float imageScales[IMAGECOUNT];
uniform vec2 imagePositions[IMAGECOUNT];
uniform vec3 planePos;

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
	uniform float snowHeight;
	uniform float seafoamStrength;
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
};

layout(std140, binding = 3) uniform Colours {
	uniform vec3 dirtColour;
	uniform vec3 mountainColour;
	uniform vec3 grassColour;
	uniform vec3 snowColour;
	uniform vec3 waterColour;
	uniform vec3 sunColour;
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

vec3 perlin(vec2 pos, int reroll) {
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

vec4 getTerrainInfo(vec2 worldPos) {
	for (int i = 0; i < IMAGECOUNT; ++i) {
		vec2 sampleCoord = ((worldPos / terrainScale - imagePositions[i]) / imageScales[i]) + vec2(0.5);
		
		if (!(sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0)) {
			vec4 terrainInfo = texture(images[i], sampleCoord);
			terrainInfo.yz /= imageScales[i] * terrainScale;
			return terrainInfo;
		}
	}
	return vec4(0, 0, 0, 0);
}

float easeInOutQuint(float x) {
	return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2.0 * x + 2, 5.0) / 2;
}

void main() {
	// Water
	vec2 flatWorldPos = worldPos3.xz;
	vec3 waterInfo = getWaterHeight(flatWorldPos);
	vec4 terrainInfo = getTerrainInfo(flatWorldPos);

	// Lighting
	vec3 normal = normalize(vec3(-waterInfo.y, 1, -waterInfo.z));
	float diffuse = max(0, dot(dirToLight, normal));

	vec3 viewDir = normalize(cameraPos - worldPos3);
	vec3 halfWay = normalize(viewDir + dirToLight);
	float spec = pow(max(dot(normal, halfWay), 0), specExp);

	float ambient = 0.2;


	vec3 albedo = waterColour;
	float nearTerrainFactor = 1 - (abs(waterInfo.x + planePos.y - terrainInfo.x)) / seafoamStrength;
	nearTerrainFactor = clamp(nearTerrainFactor, 0.0, 1.0);
	if (nearTerrainFactor > 0) {
		float whiteStrength = nearTerrainFactor;
		//float whiteStrength = perlin(flatWorldPos, 0).x;
		//whiteStrength = easeInOutQuint(whiteStrength) * nearTerrainFactor;
		albedo = vec3(0.7) * whiteStrength + albedo * (1 - whiteStrength);
	}

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
	vec3 reflectColour = texture(skybox, reflectDir).xyz;

	fresnel = clamp(fresnel, 0.0, 1.0);
	litAlbedo = fresnel * reflectColour + (1 - fresnel) * litAlbedo + spec * sunColour;

	vec3 finalColour = (1 - fogStrength) * litAlbedo + fogStrength * texture(skybox, skyboxSample).xyz;

	FragColour = vec4(finalColour, 1);
}