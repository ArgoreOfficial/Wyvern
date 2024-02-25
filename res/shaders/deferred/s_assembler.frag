#version 430 core

out vec4 FragColor;

in vec2 TexCoord;

layout(binding = 0) uniform sampler2D light_Position;
layout(binding = 1) uniform sampler2D light_Normal;
layout(binding = 2) uniform sampler2D light_Albedo;
layout(binding = 3) uniform sampler2D light_MetallicRoughness;
layout(binding = 4) uniform sampler2D light_Depth;

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
   else if( uRenderMode == 4 ) FragColor.rgb = texture( light_Depth,             TexCoord ).rgb;
   
   FragColor.a = 1.0f;
}