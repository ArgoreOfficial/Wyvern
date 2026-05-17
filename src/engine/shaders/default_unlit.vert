#version 450
#extension GL_ARB_shading_language_include : require
#extension GL_EXT_buffer_reference : require

struct Vertex {
	float normal[3];
	float color[3];
	float texCoord0[2];
}; 

struct MaterialData {	
	vec4 albedoColor;
	uint albedoIndex;
};

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTexCoord0;

layout (location = 3) flat out uint outAlbedoIndex;

#include "vertex.glsl"

void main() 
{
	MaterialData material = getMaterial(materialIndex);
	
	Vertex v = getVertex(gl_VertexIndex);
	vec3 pos = getPosition(gl_VertexIndex);
	
	//output the position of each vertex
	gl_Position = sceneData.viewProjMatrix * modelMatrix * vec4(pos, 1.0f);
	
	outColor       = unpackFloat3(v.color) * material.albedoColor.rgb;
	outTexCoord0   = unpackFloat2(v.texCoord0);
	outAlbedoIndex = material.albedoIndex;
}