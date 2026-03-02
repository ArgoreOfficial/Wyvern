#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 1) uniform sampler2D u_globalTextures2D[];
layout(set = 0, binding = 1) uniform samplerCube u_globalTexturesCube[];

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTexCoord0;
layout (location = 2) in vec3 inNormal;

layout (location = 3) flat in uint inAlbedoIndex;

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec3 normal = normalize(inNormal);
	if( !gl_FrontFacing )
		normal *= -1.0; // if back face culling is off, we need to flip the normal

	float light = dot(normal, normalize(vec3(1,3,2)));
	light = max(light, 0.0);
	light = light * 0.8 + 0.2;

	vec4 color = texture(u_globalTextures2D[inAlbedoIndex], inTexCoord0);
	float alpha = color.a;
	if( alpha < 0.5 )
		discard;

	outFragColor = vec4(color.rgb *= light, alpha ) * vec4(inColor, 1.0);
	outFragColor.a = alpha;
}