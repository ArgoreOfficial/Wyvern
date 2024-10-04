#extension GL_ARB_bindless_texture : enable

#if GL_ES 
precision mediump float;
#endif

in vec2 TexCoord;
in vec3 Normal;
in vec3 Pos;
in flat sampler2D Albedo;
in flat int HasAlpha;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Normal;
layout(location = 2) out vec4 o_RoughnessMetallic;

void main()
{
    o_Albedo = texture( Albedo, TexCoord );
    o_Normal = vec4( 0.0 );
    o_RoughnessMetallic = vec4( 1.0 );
}