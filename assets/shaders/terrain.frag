#version 430 core
#define PI 3.141592653589793238462
#define IMAGECOUNT 4

in vec3 viewPos;
in vec3 groundWorldPos;
in vec3 shellWorldPos;
out vec4 FragColor;

// Per app probably
uniform int imageCount;
uniform sampler2D images[IMAGECOUNT];
uniform samplerCube skybox;
uniform vec3 cameraPos;
uniform vec3 dirToLight;

// Per whenever they get changed
uniform float imageScales[IMAGECOUNT];
uniform vec2 imagePositions[IMAGECOUNT];

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

layout(std140, binding = 3) uniform Colours {
	uniform vec3 dirtColor;
	uniform vec3 mountainColor;
	uniform vec3 grassColor;
	uniform vec3 snowColor;
	uniform vec3 waterColor;
};

uniform float waterHeight;

// Per plane
in flat int shellIndex;

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


float easeInExpo(float x) {
	return x == 0 ? 0 : pow(2.0, 10 * x - 10);
}

float easeInOutQuint(float x) {
	return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2.0 * x + 2, 5.0) / 2;
}

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

int getClosestInt(float x) {
	return int(round(x) + 0.1 * (x < 0 ? -1 : 1));
}

vec2 randUnitVector(float randNum) {
	float theta = 2 * PI * randNum;
	return normalize(vec2(cos(theta), sin(theta)));
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

float quintic(float x) {
	return x < 0.5 ? (16 * x * x * x * x * x) : 1 - pow(-2 * x + 2, 5.0) / 2.0;
}

void main() {
	bool isShell = shellIndex >= 0;

	// Terrain
	vec2 flatWorldPos = groundWorldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));

	// Shell info
	vec2 shellCoord = flatWorldPos * shellDetail;
	int shellGridX = getClosestInt(floor(shellCoord.x));
	int shellGridZ = getClosestInt(floor(shellCoord.y));
	float randNum = randToFloat(rand(labelPoint(shellGridX, shellGridZ)));
	
	// Terrain at center of shell texel
	vec2 shellWorldMiddlePos = vec2(shellGridX / shellDetail, shellGridZ / shellDetail);
	vec4 shellMiddleTerrainInfo = getTerrainInfo(shellWorldMiddlePos);

	float mountain = quintic(terrainInfo.a);
	vec3 groundAlbedo = dirtColor * (1 - mountain) + mountain * mountainColor;
	mountain = quintic(terrainInfo.a);
	mountain = quintic(terrainInfo.a);
	mountain = quintic(terrainInfo.a);
	mountain = quintic(terrainInfo.a);
	mountain = quintic(terrainInfo.a);
	vec3 shellAlbedo = grassColor * (1 - mountain) + mountain * snowColor;

	// Lighting
	float diffuse = max(0, dot(dirToLight, normal));
	float ambient = 0.2;

	// Water
	float wetHeight = 0.4;
	float distAboveWater = (isShell ? shellMiddleTerrainInfo.x : groundWorldPos.y) - waterHeight;
	float wet =  1 - (distAboveWater / wetHeight);
	wet = clamp(wet, 0.0, 1.0);

	bool shallowEnough = dot(normal, vec3(0, 1, 0)) >= colorDotCutoff;

	// Fog
	float distFromCamera = length(viewPos);
	float fogStart = maxViewDistance - fogEncroach;
	float fogStrength;

	if (distFromCamera < fogStart)
		fogStrength = 0;
	else if (distFromCamera > maxViewDistance)
		fogStrength = 1;
	else
		fogStrength = (distFromCamera - fogStart) / fogEncroach;

	/// Coloring
	vec3 albedo;

	// Regular
	if (!isShell) {
		if (wet == 0)
			albedo = shallowEnough ? shellAlbedo : groundAlbedo;
		else {
			albedo = groundAlbedo - wet * groundAlbedo * 0.4;
		}
	}

	// Shell
	else {
		float shellProgress = float(shellIndex + 1) / shellCount;
		float shellCutoff = shellBaseCutoff + shellProgress * (shellMaxCutoff - shellBaseCutoff);
		float circleDist = (1 - shellProgress) / 2.0;
		if (!shallowEnough || randNum < shellCutoff || wet > 0)
			discard;
			
		albedo = shellAlbedo + shellAlbedo * shellProgress * 0.1;
	}

	vec3 litAlbedo = (diffuse + ambient) * albedo;
	vec3 skyboxSample = shellWorldPos - cameraPos;
	vec3 finalColor = (1 - fogStrength) * litAlbedo + fogStrength * texture(skybox, skyboxSample).xyz;
	FragColor = vec4(finalColor, 1);
}