#version 430 core

layout(location = 0) in vec2 vPos;

uniform mat4 view;
uniform mat4 proj;

out vec3 fragColor;

void main() {
	gl_Position = proj * view * vec4(vPos.x, 0, vPos.y, 1);
}