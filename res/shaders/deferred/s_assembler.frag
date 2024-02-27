#version 430 core

out vec4 FragColor;

in vec2 TexCoord;

layout(binding = 0) uniform sampler2D light_Position;
layout(binding = 1) uniform sampler2D light_Normal;
layout(binding = 2) uniform sampler2D light_Albedo;
layout(binding = 3) uniform sampler2D light_MetallicRoughness;
layout(binding = 4) uniform sampler2D light_Depth;
layout(binding = 5) uniform sampler2D light_Light;

// CONSTS

const float GAMMA    = 2.2;
const float EXPOSURE = 0.04;

// UNIFORMS

layout (std140) uniform uboRenderInfo
{
   int uRenderMode;
};

//

vec3 gammaCorrect( vec3 _col )
{
    return pow( _col, vec3( 1.0 / GAMMA ) );
}

vec3 tonemap( vec3 _col )
{
    return vec3(1.0) - exp(-_col * EXPOSURE);
}

vec3 combineRender() 
{

	vec4 albedo = texture( light_Albedo,   TexCoord );
	albedo.xyz  = pow    ( albedo.xyz, vec3( GAMMA ) ); // gamma correction
	vec4 light  = texture( light_Light, TexCoord ); 

    vec3 surface_color = tonemap( albedo.xyz * light.xyz );
    surface_color = gammaCorrect( surface_color );

	return surface_color;
}

void main()
{

		 if( uRenderMode == 0 ) FragColor.rgb = combineRender();
	else if( uRenderMode == 1 ) FragColor.rgb = texture( light_Albedo,            TexCoord ).rgb;
	else if( uRenderMode == 2 ) FragColor.rgb = texture( light_Normal,            TexCoord ).rgb;
	else if( uRenderMode == 3 ) FragColor.rgb = texture( light_Position,          TexCoord ).rgb;
	else if( uRenderMode == 4 ) FragColor.rgb = texture( light_MetallicRoughness, TexCoord ).ggg;
	else if( uRenderMode == 5 ) FragColor.rgb = texture( light_MetallicRoughness, TexCoord ).bbb;
	else if( uRenderMode == 6 ) FragColor.rgb = texture( light_Depth,             TexCoord ).rgb;
	else if( uRenderMode == 7 ) FragColor.rgb = tonemap(texture( light_Light,             TexCoord ).rgb);
	
	FragColor.a = 1.0f;
}