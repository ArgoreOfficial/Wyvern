#if GL_ES 
precision mediump float;
#endif

/// TODO: reflect to CPU so binding=0 doesn't need to be used
#if GL_ES 
uniform sampler2D u_Texture;
#else
layout(binding = 0) uniform sampler2D u_Texture;
#endif

in vec2 TexCoord;
in vec3 Normal;
in vec3 Pos;

layout(location = 0) out vec4 u_Albedo;
layout(location = 1) out vec4 u_Normal;
layout(location = 2) out vec4 u_Position;
layout(location = 3) out vec4 u_RoughnessMetallic;

void main()
{
    vec3 normalColor = (Normal / 2.0) + vec3( 0.5 );
    
    u_Albedo = vec4( normalColor, 1.0 ); //texture( u_Texture, TexCoord );
    u_Normal = vec4( Normal, 1.0 );
    u_Position = vec4( Pos, 1.0 );
    u_RoughnessMetallic = vec4( 1.0 );
}