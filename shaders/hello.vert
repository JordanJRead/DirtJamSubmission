#version 430 core

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec3 vColor;

uniform mat4 view;
uniform mat4 proj;

out vec3 fragColor;

void main() {
	gl_Position = proj * view * vec4(vPos, 0, 1);
	fragColor = vColor;
}