#version 430 core

layout(location = 0) in vec2 vPos; // -1 to 1

out vec2 latticePos;

uniform float scale;
uniform vec2 worldPos;

void main() {
	latticePos = (0.5 * vPos) * scale + worldPos;
	gl_Position = vec4(vPos, 0, 1);
}