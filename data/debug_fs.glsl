#version 460 core

in vec4 vertexColor;
in vec2 texCoord0;

out vec4 FragColor;

void main() {
	FragColor = vec4(texCoord0.x, texCoord0.y, 0, 1);
}