#version 460 core
//#version 300 es

struct VertexData { 
	float normal[ 3 ]; 
	float vertexColor[ 3 ]; 
	float texCoord0[ 2 ]; 
};

layout(binding = 0, std430) readonly buffer ssbopos { float[3] positions[]; };
layout(binding = 1, std430) readonly buffer ssbodat { VertexData vertexDatas[]; };

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

layout(location = 2) uniform mat4 u_view_proj_matrix;
layout(location = 3) uniform mat4 u_model_matrix;
layout(location = 4) uniform vec2 u_texcoord_offset;

out vec4 vertexColor;
out vec2 texcoord;

out gl_PerVertex{ vec4 gl_Position; };

void main() {
	texcoord = getTexCoord0(gl_VertexID) + u_texcoord_offset;
	vertexColor = getVertexColor(gl_VertexID);
	gl_Position = u_view_proj_matrix * u_model_matrix * vec4( getPosition(gl_VertexID).xyz, 1.0 );
}