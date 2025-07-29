#version 430 core

in vec3 outVPos;

out vec4 FragColor;

uniform samplerCube skybox;

void main() {
	FragColor = texture(skybox, outVPos);
}