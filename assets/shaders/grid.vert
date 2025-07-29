#version 430 core

layout(location = 0) in vec2 vPos;

out vec3 worldPos;
out vec3 viewPos;

uniform mat4 view;
uniform mat4 proj;
uniform float scale;

void main() {
	worldPos = vec3(vPos.x * scale, 0, vPos.y * scale);
	gl_Position = proj * view * vec4(worldPos, 1);
	viewPos = (view * vec4(worldPos, 1)).xyz;
}