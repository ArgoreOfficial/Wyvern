#version 460 core
//#version 300 es

struct VertexData { 
	float normal[ 3 ]; 
	float vertexColor[ 3 ]; 
	float texCoord0[ 2 ]; 
};

layout(binding = 0, std430) readonly buffer ssbo_positionData { float[3] positions[]; };
layout(binding = 1, std430) readonly buffer ssbo_vertexData   { VertexData vertexDatas[]; };

layout (binding = 2, std140) uniform ubo_sceneData
{
    mat4 u_viewProjMatrix;
};

layout (binding = 3, std140) uniform ubo_materialData
{
	mat4 u_modelMatrix;
};


vec3 getPosition(int _idx) {
	return vec3(
		positions[_idx][0], 
		positions[_idx][1], 
		positions[_idx][2]
	);
}

vec4 getVertexColor(int _idx) {
	return vec4(
		vertexDatas[_idx].vertexColor[0],
		vertexDatas[_idx].vertexColor[1],
		vertexDatas[_idx].vertexColor[2],
		1.0
	);
}

vec2 getTexCoord0(int _idx) {
	return vec2(
		vertexDatas[_idx].texCoord0[0], 
		vertexDatas[_idx].texCoord0[1]
	);
}

out vec4 vertexColor;
out vec2 texCoord0;

out gl_PerVertex{ vec4 gl_Position; };

void main() {
	texCoord0 = getTexCoord0(gl_VertexID);
	vertexColor = getVertexColor(gl_VertexID);
	gl_Position = u_viewProjMatrix * u_modelMatrix * vec4( getPosition(gl_VertexID).xyz, 1.0 );
}