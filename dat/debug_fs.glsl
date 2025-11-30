#version 460 core
//#version 300 es

uniform sampler2D u_texture;

in vec2 texcoord;
out vec4 FragColor;

void main() {
	FragColor = texture(u_texture, texcoord);

	if( FragColor.a < 0.1 )
		discard;
}