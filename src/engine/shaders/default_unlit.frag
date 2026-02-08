#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 1) uniform sampler2D u_globalTextures2D[];
layout(set = 0, binding = 1) uniform samplerCube u_globalTexturesCube[];

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTexCoord0;

layout (location = 3) flat in uint inAlbedoIndex;

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec4 color = texture(u_globalTextures2D[inAlbedoIndex], inTexCoord0);
	if( color.a < 0.5 )
		discard;

	outFragColor = color;
}