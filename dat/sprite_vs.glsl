#version 300 es
//#version 460 core

struct VertexData { 
	float position[ 3 ]; 
	float uv[ 2 ]; 
};

layout(binding = 0, std430) readonly buffer ssbo1 { 
	VertexData data[]; 
};

vec2 getPosition( int index ) { 
	return vec2( 
		data[ index ].position[ 0 ], 
		data[ index ].position[ 1 ] 
	); 
}

vec2 getUV( int index ) { 
	return vec2( 
		data[ index ].uv[ 0 ], 
		data[ index ].uv[ 1 ] 
	); 
}

layout(location = 1) uniform mat4 u_view_proj_matrix;
layout(location = 2) uniform mat4 u_model_matrix;
layout(location = 3) uniform vec2 u_texcoord_offset;

out gl_PerVertex{ vec4 gl_Position; };
out vec2 texcoord;

void main() {
	texcoord = getUV(gl_VertexID) + u_texcoord_offset;
	gl_Position = u_view_proj_matrix * u_model_matrix * vec4( getPosition(gl_VertexID).xy, 0.0, 1.0 );
}