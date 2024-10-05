#extension GL_ARB_bindless_texture : enable

#if GL_ES 
precision mediump float;
#endif

in vec2 TexCoord;
in vec3 Normal;
in vec3 Pos;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec2 o_Normal;
layout(location = 2) out vec2 o_RoughnessMetallic;

void main()
{
    o_Albedo = vec4( 1.0, 0.0, 1.0, 1.0 );
    o_Normal = Normal.xy;
    o_RoughnessMetallic = vec2( 1.0 );
}