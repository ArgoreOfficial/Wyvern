#version 450
#extension GL_ARB_shading_language_include : require
#extension GL_EXT_buffer_reference : require

#include "vertex.glsl"

struct Vertex {
	float normal[3];
	float color[3];
	float texCoord0[2];
}; 

struct MaterialData {
	vec4 color;
};

DEFINE_POSITION_BUFFER();
DEFINE_VERTEX_BUFFER(Vertex);
DEFINE_MATERIAL_BUFFER(MaterialData);

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform pushConstant {
    mat4 viewProjMatrix;
    mat4 modelMatrix;
	PositionBuffer positionBuffer;
	VertexBuffer vertexBuffer;
	MaterialBuffer materialBuffer;
	uint materialIndex;
};

void main() 
{
	vec3 pos = getPosition(gl_VertexIndex);
	
	//output the position of each vertex
	gl_Position = viewProjMatrix * modelMatrix * vec4(pos, 1.0f);
	
	outColor = getMaterial(materialIndex).color;
}