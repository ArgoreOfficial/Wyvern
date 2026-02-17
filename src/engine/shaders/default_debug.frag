#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 1) uniform sampler2D u_globalTextures2D[];
layout(set = 0, binding = 1) uniform samplerCube u_globalTexturesCube[];

//shader input
layout (location = 0) in vec4 inColor;

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = inColor;
}