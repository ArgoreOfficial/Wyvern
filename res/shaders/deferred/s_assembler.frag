#version 410 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D light_Position;
uniform sampler2D light_Normal;
uniform sampler2D light_Albedo;
uniform sampler2D light_MetallicRoughness;
uniform sampler2D light_Depth;

layout (std140) uniform uboRenderInfo
{
   int uRenderMode;
};

void main()
{
         if( uRenderMode == 0 ) FragColor.rgb = texture( light_Position,          TexCoord ).rgb;
    else if( uRenderMode == 1 ) FragColor.rgb = texture( light_Normal,            TexCoord ).rgb;
    else if( uRenderMode == 2 ) FragColor.rgb = texture( light_Albedo,            TexCoord ).rgb;
    else if( uRenderMode == 3 ) FragColor.rgb = texture( light_MetallicRoughness, TexCoord ).rgb;
    else if( uRenderMode == 4 ) FragColor.rgb = texture( light_Depth,             TexCoord ).rrr;

    FragColor.a = 1.0f;
}