#version 430 core

in vec3 worldPos;
in vec3 viewPos;

out vec4 FragColor;

int getClosestInt(float x) {
	return int(round(x) + 0.1 * (x < 0 ? -1 : 1));
}

void main() {
	if (length(worldPos) < 0.1) {
		FragColor = vec4(1, 1, 1, 1);
		return;
	}

	int closestX = getClosestInt(worldPos.x);
	int closestZ = getClosestInt(worldPos.z);

	if (abs(worldPos.x - closestX) < 0.05 || abs(worldPos.z - closestZ) < 0.05) {
		vec4 lineColor = vec4(1, 1, 1, 1) / 3;
		vec4 black = vec4(0, 0, 0, 0);

		int strength = (closestX % 10 == 0 || closestZ % 10 == 0) ? 3 : 2;
		float fogScale = 1 - strength * pow(0.9, length(viewPos));
		FragColor = black * fogScale + lineColor * (1 - fogScale);
	}
	else {
		discard;
	}
}