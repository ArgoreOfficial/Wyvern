#version 450
#extension GL_ARB_shading_language_include : require
#extension GL_EXT_buffer_reference : require

#include "vertex.glsl"

struct Vertex {
	float normal[3];
	float color[3];
	float texCoord0[2];
}; 

DEFINE_POSITION_BUFFER();
DEFINE_VERTEX_BUFFER(Vertex);

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTexCoord0;
layout (location = 2) out vec3 outNormal;

layout (location = 3) flat out uint outAlbedoIndex;

layout(push_constant) uniform pushConstant {
    mat4 viewProjMatrix;
    mat4 modelMatrix;
	PositionBuffer positionBuffer;
	VertexBuffer vertexBuffer;

	uint albedoIndex;
	vec4 albedoColor;
};

void main() 
{
	Vertex v = getVertex(gl_VertexIndex);
	vec3 pos = getPosition(gl_VertexIndex);
	
	//output the position of each vertex
	gl_Position = viewProjMatrix * modelMatrix * vec4(pos, 1.0f);
	
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	outColor     = unpackFloat3(v.color) * albedoColor.rgb;
	outTexCoord0 = unpackFloat2(v.texCoord0);
	outNormal    = normalMatrix * unpackFloat3(v.normal);

	outAlbedoIndex = albedoIndex;
}