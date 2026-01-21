#version 450
#extension GL_ARB_shading_language_include : require
#extension GL_EXT_buffer_reference : require

#include "vertex.glsl"

struct Vertex {
	float posX;
	float posY;
	float posZ;
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
	vec3 pos = getPosition(gl_VertexIndex);
	
	//const array of colors for the triangle
	const vec3 colors[3] = vec3[3](
		vec3(1.0f, 0.0f, 0.0f), //red
		vec3(0.0f, 1.0f, 0.0f), //green
		vec3(00.f, 0.0f, 1.0f)  //blue
	);

	//output the position of each vertex
	gl_Position = worldMatrix * vec4(pos, 1.0f);
	outColor = colors[gl_VertexIndex % 3];
}