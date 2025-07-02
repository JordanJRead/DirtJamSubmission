#version 430 core

in flat float n;
in vec3 color;
out vec4 FragColor;

void main() {
	FragColor = vec4(n, n, n, 1);
	//FragColor = vec4(color, 1);
}