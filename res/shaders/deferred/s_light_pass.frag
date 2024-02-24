#version 410 core

layout (location = 0) out vec4  light_Position;
layout (location = 1) out vec4  light_Normal;
layout (location = 2) out vec4  light_Albedo;
layout (location = 3) out vec4  light_MetallicRoughness;
layout (location = 4) out float light_Depth;

in vec2 TexCoord;

uniform sampler2D gbuffer_Position;
uniform sampler2D gbuffer_Normal;
uniform sampler2D gbuffer_Albedo;
uniform sampler2D gbuffer_MetallicRoughness;
uniform sampler2D gbuffer_Depth;

void main()
{
    light_Position          = texture( gbuffer_Position,          TexCoord );
    light_Normal            = texture( gbuffer_Normal,            TexCoord );
    light_Albedo            = texture( gbuffer_Albedo,            TexCoord );
    light_MetallicRoughness = texture( gbuffer_MetallicRoughness, TexCoord );
    light_Depth             = texture( gbuffer_Depth,             TexCoord ).r;
}