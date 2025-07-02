#version 430 core

layout(location = 0) in vec2 vPos;

uniform mat4 view;
uniform mat4 proj;
uniform float scale;

void main() {
	gl_Position = proj * view * vec4(vPos.x * scale, 0, vPos.y * scale, 1);
}