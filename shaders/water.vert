#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

out vec2 flatWorldPos;
out vec3 viewPos;

// Per frame
uniform mat4 view;
uniform mat4 proj;

// Per plane
uniform float planeWorldWidth;
uniform vec3 planePos;

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + planePos.x, planePos.y, vPos.y * planeWorldWidth + planePos.z, 1);
	flatWorldPos = worldPos.xz;

	viewPos = (view * worldPos).xyz;
	gl_Position = proj * vec4(viewPos, 1);
}