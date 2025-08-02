#version 430 core
#define PI 3.141592653589793238462
#define IMAGECOUNT 4

in vec3 viewPos;
in vec3 groundWorldPos;
in vec3 shellWorldPos;
out vec4 FragColor;

// Per app probably
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
	uniform float snowHeight;
	uniform float seafoamStrength;
};

layout(std140, binding = 3) uniform Colours {
	uniform vec3 dirtColor;
	uniform vec3 mountainColor;
	uniform vec3 grassColor;
	uniform vec3 snowColor;
	uniform vec3 waterColor;
	uniform vec3 sunColour;
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

float quintic(float x) {
	return x < 0.5 ? (16 * x * x * x * x * x) : 1 - pow(-2 * x + 2, 5.0) / 2.0;
}

float expo(float x) {
	return x == 1 ? 1 : 1 - pow(2.0, -10.0 * x);
}

void main() {
	bool isShell = shellIndex >= 0;

	// Terrain
	vec2 flatWorldPos = groundWorldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos);
	float height = groundWorldPos.y;
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));

	// Shell info
	vec2 shellCoord = flatWorldPos * shellDetail;
	int shellGridX = getClosestInt(floor(shellCoord.x));
	int shellGridZ = getClosestInt(floor(shellCoord.y));
	float randNum = randToFloat(rand(labelPoint(shellGridX, shellGridZ)));
	
	// Terrain at center of shell texel
	vec2 shellWorldMiddlePos = vec2(shellGridX / shellDetail, shellGridZ / shellDetail);
	vec4 shellMiddleTerrainInfo = getTerrainInfo(shellWorldMiddlePos);

	float mountain = terrainInfo.a;
	vec3 groundAlbedo = dirtColor * (1 - mountain) + mountain * mountainColor;
	mountain = quintic(mountain);
	mountain = expo(mountain);
	mountain = quintic(mountain);
	mountain = quintic(mountain);
	if (snowHeight > height)
		mountain = 0;

	vec3 shellAlbedo = grassColor * (1 - mountain) + mountain * snowColor;

	// Lighting
	float diffuse = max(0, dot(dirToLight, normal));
	float ambient = 0.2;
	vec3 viewDir = normalize(cameraPos - groundWorldPos);
	vec3 halfWay = normalize(viewDir + dirToLight);
	float spec = isShell ? 0 : pow(max(dot(normal, halfWay), 0), specExp);

	// Water
	float wetHeight = 0.4;
	float distAboveWater = (isShell ? shellMiddleTerrainInfo.x : groundWorldPos.y) - waterHeight;
	float wet =  1 - (distAboveWater / wetHeight);
	wet = clamp(wet, 0.0, 1.0);
	spec *= wet * wet;

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

	vec3 litAlbedo = (diffuse + ambient) * albedo + spec * sunColour;
	vec3 skyboxSample = shellWorldPos - cameraPos;
	vec3 finalColor = (1 - fogStrength) * litAlbedo + fogStrength * texture(skybox, skyboxSample).xyz;
	FragColor = vec4(finalColor, 1);
}