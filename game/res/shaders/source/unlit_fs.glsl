#if GL_ES 
precision mediump float;
#endif

/// TODO: reflect to CPU so binding=0 doesn't need to be used
#if GL_ES 
uniform sampler2D u_Albedo;
#else
layout(binding = 0) uniform sampler2D u_Albedo;
#endif

in vec2 TexCoord;
in vec3 Normal;
in vec3 Pos;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Normal;
layout(location = 2) out vec4 o_Position;
layout(location = 3) out vec4 o_RoughnessMetallic;

void main()
{
    o_Albedo = texture( u_Albedo, TexCoord );
    o_Normal = vec4( Normal, 1.0 );
    o_Position = vec4( Pos, 1.0 );
    o_RoughnessMetallic = vec4( 1.0 );
}