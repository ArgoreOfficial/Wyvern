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

layout(push_constant) uniform pushConstant {
    mat4 worldMatrix;
	PositionBuffer positionBuffer;
	VertexBuffer vertexBuffer;
};

void main() 
{
	Vertex v = getVertex(gl_VertexIndex);
	vec3 pos = getPosition(gl_VertexIndex);
	
	//output the position of each vertex
	gl_Position = worldMatrix * vec4(pos, 1.0f);
	outColor = unpackFloat3(v.normal);
}