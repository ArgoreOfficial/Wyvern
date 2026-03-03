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
	vec3 lightDir = normalize(vec3(1,3,2));

	if( !gl_FrontFacing )
		normal *= -1.0; // if back face culling is off, we need to flip the normal

	vec4 color = texture(u_globalTextures2D[inAlbedoIndex], inTexCoord0);
	float alpha = color.a;
	if( alpha < 0.5 )
		discard;

	// Lighting stuff

	float ambientStrength = 0.4;
	vec3 ambientColor = vec3(1, 1, 1);
	vec3 ambient = ambientStrength * ambientColor;

	float diffuseStrength = max(dot(normal, lightDir), 0.0);
	vec3 diffuseColor = vec3(1, 1, 1);
	vec3 diffuse = diffuseStrength * diffuseColor;

	vec3 finalColor = (ambient + diffuse) * color.rgb;

	outFragColor.rgb = finalColor * inColor;
	outFragColor.a = alpha;
}