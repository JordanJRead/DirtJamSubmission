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

uniform float waterHeight;

// Per plane
in flat int shellIndex;

vec3 getTerrainInfo(vec2 worldPos) {
	for (int i = 0; i < IMAGECOUNT; ++i) {
		vec2 sampleCoord = ((worldPos / terrainScale - imagePositions[i]) / imageScales[i]) + vec2(0.5);
		
		if (!(sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0)) {
			vec3 terrainInfo = texture(images[i], sampleCoord).rgb;
			terrainInfo.yz /= imageScales[i] * terrainScale;
			return terrainInfo;
		}
	}
	return vec3(0, 0, 0);
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

void main() {

	// Terrain
	vec2 flatWorldPos = groundWorldPos.xz;
	vec3 terrainInfo = getTerrainInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));

	// Color
	vec3 dirtAlbedo = vec3(0.61, 0.46, 0.33) * 0.7;
	vec3 grassAlbedo = vec3(0, 0.5, 0);
	vec3 snowAlbedo = vec3(1, 1, 1);
	vec3 rockAlbedo = vec3(0.4, 0.4, 0.4);

	float tall = (groundWorldPos.y - 40) / 10.0;
	tall = clamp(tall, 0.0, 1.0);
	vec3 groundAlbedo = dirtAlbedo;// * (1 - tall) + tall * rockAlbedo;
	
	vec3 shellAlbedo = grassAlbedo;// * (1 - tall) + tall * snowAlbedo;

	// Lighting
	vec3 lightDir = normalize(vec3(0, 1, 0));
	float diffuse = max(0, dot(lightDir, normal));
	float ambient = 0;

	// Texturing
	vec2 shellCoord = flatWorldPos * shellDetail;
	int x = getClosestInt(floor(shellCoord.x));
	int y = getClosestInt(floor(shellCoord.y));
	float randNum = randToFloat(rand(labelPoint(x, y)));
	vec2 normGrass = shellCoord - vec2(x, y);

	bool shallowEnough = diffuse >= colorDotCutoff;

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
	bool underwater = groundWorldPos.y - 0.2 < waterHeight;
	if (shellIndex < 0) {
		albedo = shallowEnough && !underwater ? shellAlbedo : groundAlbedo;
	}

	// Shell
	else {
		float shellProgress = float(shellIndex + 1) / shellCount;
		float shellCutoff = shellBaseCutoff + shellProgress * (shellMaxCutoff - shellBaseCutoff);
		float circleDist = (1 - shellProgress) / 2.0;
		if (!shallowEnough || randNum < shellCutoff || underwater)// || length(vec2(0.5, 0.5) - normGrass) > circleDist) // If doing cones
			discard;
			
		albedo = shellAlbedo + shellAlbedo * shellProgress * 0.1;
	}

	vec3 litAlbedo = (diffuse + ambient) * albedo;
	vec3 skyboxSample = shellWorldPos - cameraPos;
	vec3 finalColor = (1 - fogStrength) * litAlbedo + fogStrength * texture(skybox, skyboxSample).xyz;
	FragColor = vec4(finalColor, 1);
}