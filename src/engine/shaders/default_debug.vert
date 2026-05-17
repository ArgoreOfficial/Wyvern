#version 450
#extension GL_ARB_shading_language_include : require
#extension GL_EXT_buffer_reference : require

struct Vertex {
	float normal[3];
	float color[3];
	float texCoord0[2];
}; 

struct MaterialData {
	vec4 color;
};

layout (location = 0) out vec4 outColor;

#include "vertex.glsl"

void main() 
{
	vec3 pos = getPosition(gl_VertexIndex);
	
	//output the position of each vertex
	gl_Position = sceneData.viewProjMatrix * modelMatrix * vec4(pos, 1.0f);
	
	outColor = getMaterial(materialIndex).color;
}