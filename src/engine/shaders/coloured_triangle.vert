#version 450

layout (location = 0) out vec3 outColor;

layout(push_constant) uniform _PushConstant {
    float offsetX;
    float offsetY;
    float offsetZ;
	uint pad0;
};

void main() 
{
	//const array of positions for the triangle
	const vec3 positions[3] = vec3[3](
		vec3( 0.5f, 0.5f, 0.0f),
		vec3(-0.5f, 0.5f, 0.0f),
		vec3( 0.0f,-0.5f, 0.0f)
	);

	//const array of colors for the triangle
	const vec3 colors[3] = vec3[3](
		vec3(1.0f, 0.0f, 0.0f), //red
		vec3(0.0f, 1.0f, 0.0f), //green
		vec3(00.f, 0.0f, 1.0f)  //blue
	);

	//output the position of each vertex
	gl_Position = vec4(positions[gl_VertexIndex] + vec3(offsetX, offsetY, offsetZ), 1.0f);
	outColor = colors[gl_VertexIndex];
}