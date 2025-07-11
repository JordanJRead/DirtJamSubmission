#version 430 core
#define PI 3.141592653589793238462

in vec2 flatWorldPos;
in vec3 viewPos;
out vec4 FragColor;

// Per app probably
uniform int imageCount;
uniform sampler2D images[];

// Per whenever they get changed
uniform float imageScales[];
uniform vec2 imagePositions[];

layout(std140, binding = 1) uniform ArtisticParams {
	uniform float extrudePerShell;
	uniform float maxFogDist;
	uniform float colorDotCutoff;
	uniform float shellTexelScale;
	uniform float cutoffLossPerShell;
	uniform float cutoffBase;
	uniform int maxShellCount;
};

// Per plane
uniform int shellIndex;

vec3 getTerrainInfo(vec2 worldPos) {
	for (int i = 0; i < imageCount; ++i) {
		vec2 sampleCoord = ((worldPos - imagePositions[i]) / imageScales[i]) + vec2(0.5);
		
		if (!(sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0)) {
			vec3 terrainInfo = texture(images[i], sampleCoord).rgb;
			terrainInfo.yz /= imageScales[i];
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
	vec3 terrainInfo = getTerrainInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));

	// Color
	vec3 dirtAlbedo = vec3(0.61, 0.46, 0.33);
	//vec3 dirtAlbedo = vec3(0.35, 0.35, 0.35);

	vec3 grassAlbedo = vec3(0, 0.6, 0);
	//vec3 grassAlbedo = vec3(1, 1, 1);

	// Lighting
	vec3 lightDir = normalize(vec3(0, 1, 0));
	float diffuse = max(0, dot(lightDir, normal));
	float ambient = 0;

	// Texturing
	int x = getClosestInt(floor((flatWorldPos * textureScale).x));
	int y = getClosestInt(floor((flatWorldPos * textureScale).y));
	float randNum = randToFloat(rand(labelPoint(x, y)));

	bool shallowEnough = diffuse >= colorDotCutoff;

	if (shellIndex < 1) {
		// Blend color
		vec3 albedo = dirtAlbedo + easeInExpo(diffuse) * (grassAlbedo - dirtAlbedo);
		albedo = dirtAlbedo + (shallowEnough ? 1 : 0) * (grassAlbedo - dirtAlbedo);

		float distFromCamera = length(viewPos);
		float maxDist = maxFogDist;
		float visibility = 1 - easeInOutQuint(clamp(distFromCamera / maxDist, 0, 1));

		vec3 preFogColor = (diffuse + ambient) * albedo;
		vec3 postFogColor = visibility * preFogColor + (1 - visibility) * vec3(0.5, 0.5, 0.5);
		FragColor = vec4(postFogColor, 1);
	}
	else {
		if (!shallowEnough || randNum < cutoffLossPerShell * shellIndex + cutoffBase)
			discard;
			
		// Blend color
		vec3 albedo = grassAlbedo + grassAlbedo * shellIndex / float(maxShellCount) * 0.5;

		float distFromCamera = length(viewPos);
		float maxDist = maxFogDist;
		float visibility = 1 - easeInOutQuint(clamp(distFromCamera / maxDist, 0, 1));

		vec3 preFogColor = (diffuse + ambient) * albedo;
		vec3 postFogColor = visibility * preFogColor + (1 - visibility) * vec3(0.5, 0.5, 0.5);
		FragColor = vec4(postFogColor, 1);
	}
}