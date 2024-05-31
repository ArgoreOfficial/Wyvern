#if GL_ES 
precision mediump float;
#endif

uniform UbInput {
    vec3 u_Color;
    float u_Alpha;
};

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
    FragColorB = vec4( TexCoord.x, TexCoord.y, 0.0, 1.0 );
}