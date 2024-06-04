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

layout(location = 0) out vec4 FragColorA;
layout(location = 1) out vec4 FragColorB;

void main()
{
    FragColorA = texture( u_Texture, TexCoord );
    FragColorB = vec4( 1.0 );
}