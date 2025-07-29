#version 430 core

layout(location = 0) in vec3 vPos;

out vec3 outVPos;

uniform mat4 view;
uniform mat4 proj;

void main() {
	outVPos = vPos;
	gl_Position = (proj * mat4(mat3(view)) * vec4(vPos, 1)).xyww; // .xyww is to not mess with depth values
}