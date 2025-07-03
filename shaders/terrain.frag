#version 430 core

in flat float n;
out vec4 FragColor;

void main() {
	FragColor = vec4(n, n, n, 1);
}