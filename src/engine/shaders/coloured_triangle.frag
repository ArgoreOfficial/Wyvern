#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 1) uniform sampler2D u_globalTextures2D[];
layout(set = 0, binding = 1) uniform samplerCube u_globalTexturesCube[];

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTexCoord0;

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
	//return red
	// outFragColor = vec4(inColor,1.0f);
	outFragColor = texture(u_globalTextures2D[0], inTexCoord0);
}