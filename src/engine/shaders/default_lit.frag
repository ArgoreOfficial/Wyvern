#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 1) uniform sampler2D u_globalTextures2D[];
layout(set = 0, binding = 1) uniform samplerCube u_globalTexturesCube[];

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTexCoord0;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inViewToVert;
layout (location = 4) flat in uint inAlbedoIndex;

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

	// Basic blinn-phong

	vec3 diffuseColor = color.rgb * inColor;
	vec3 ambientColor = diffuseColor * 0.2;
	
	vec3 viewDir = normalize(-inViewToVert);

	float lambertian  = max(dot(lightDir, normal), 0.0);
	float specular = 0.0;

	if( lambertian > 0.0 )
	{
		vec3 halfDir = normalize(lightDir + viewDir);
		float specAngle = max(dot(halfDir, normal), 0.0);
		specular = pow(specAngle, 100.0) * 0.2;
	}

	outFragColor.rgb = ambientColor + diffuseColor * lambertian + specular;
	outFragColor.a = alpha;
}