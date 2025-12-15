#version 460 core
//#version 300 es

layout(binding = 0, std430) readonly buffer ssbo_positionData { float[3] positions[]; };

layout (binding = 1, std140) uniform ubo_sceneData
{
    mat4 u_viewProjMatrix;
};

layout (binding = 2, std140) uniform ubo_materialData
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

out gl_PerVertex{ vec4 gl_Position; };

void main() {
	gl_Position = u_viewProjMatrix * u_modelMatrix * vec4( getPosition( gl_VertexID ).xyz, 1.0 );
}