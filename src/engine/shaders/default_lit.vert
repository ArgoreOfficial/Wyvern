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
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec3 outViewToVert;
layout (location = 4) flat out uint outAlbedoIndex;

#include "vertex.glsl"

void main() 
{
	MaterialData material = getMaterial(materialIndex);
	
	Vertex v = getVertex(gl_VertexIndex);
	vec3 pos = getPosition(gl_VertexIndex);
	
	//output the position of each vertex
	gl_Position = sceneData.viewProjMatrix * modelMatrix * vec4(pos, 1.0f);
	
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	outColor       = unpackFloat3(v.color) * material.albedoColor.rgb;
	outTexCoord0   = unpackFloat2(v.texCoord0);
	outNormal      = normalMatrix * unpackFloat3(v.normal);
	outViewToVert  = vec3(modelMatrix * vec4(pos, 1.0f)) - vec3(sceneData.viewPos);

	outAlbedoIndex = material.albedoIndex;
}