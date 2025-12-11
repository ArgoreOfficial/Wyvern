#version 460 core

in vec4 vertexColor;
in vec2 texcoord;

out vec4 FragColor;

void main() {
	FragColor = vertexColor;
}